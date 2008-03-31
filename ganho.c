/**
 * A crappy C implementation to find the information gain inside a CSV
 * table.
 *
 * Advantages:
 *
 * - None known.
 *
 * Problems:
 *
 * - If the number of classes gets too high, there will be a huge memory
 *   waste in the hash tables of each attribute class;
 *
 * - Conversely, if the number of attributes is too high, it will be too
 *   slow to add new entries in the refmap (hm, actually this is not a big
 *   issue, as we are limited by IO).
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash.h"

/* classes don't need to have their names stored, just their "uniqueness" 
 * is enough */
struct class_entry {
	unsigned long count;
	struct hash_table *refmap; /* counts in which classes of the ref.
				    * attribute it appeared */
};

struct table_stats {
	unsigned long lines;
	struct hash_table **attributes;
	size_t nr_attributes;
	struct hash_table *refclasses;
};

struct class_entry *new_class_entry()
{
	struct class_entry *cl;

	cl = (struct class_entry*) malloc(sizeof(struct class_entry));
	if (!cl)
		return NULL;
	cl->count = 0;
	cl->refmap = hash_init(50);
	if (!cl->refmap) {
		free(cl);
		return NULL;
	}

	return cl;
}

void free_class_entry(struct class_entry *cl)
{
	hash_free(cl->refmap);
	free(cl);
}

void free_table_stats(struct table_stats *ts)
{
	size_t i;

	if (!ts)
		return;
	if (ts->refclasses)
		hash_free(ts->refclasses);
	if (ts->attributes)
		for (i = 0; i < ts->nr_attributes; i++)
			hash_free(ts->attributes[i]);
	free(ts->attributes);
	free(ts);
}

struct table_stats *new_table_stats(size_t nr_attributes)
{
	struct table_stats *ts;
	size_t i;

	ts = (struct table_stats*) malloc(sizeof(struct table_stats));
	if (!ts)
		return NULL;

	ts->refclasses = hash_init(1024);
	if (!ts->refclasses)
		goto failed;

	ts->attributes = (struct hash_table**)
		malloc(sizeof(struct hash_table*) * nr_attributes);
	if (!ts->attributes)
		goto failed;
	for (i = 0; i < nr_attributes; i++) {
		ts->attributes[i] = hash_init(1024);
		if (!ts->attributes[i]) {
			for (; i >= 0; i--)
				hash_free(ts->attributes[i]);
			free(ts->attributes);
			return NULL;
		}
	}

	ts->nr_attributes = nr_attributes;
	ts->lines = 0;

	return ts;
failed:
	free_table_stats(ts);
	return NULL;
}

void dump_table_stats(struct table_stats *ts)
{
	size_t i, j, k;
	struct class_entry *ce;

	/* FIXME there may be more entries in a hash entry! */

	printf("lines: %d\n", ts->lines);
	for (i = 0; i < ts->nr_attributes; i++) {
		printf("attribute %d:\n", i);
		for (j = 0; j < ts->attributes[i]->size; j++) {
			if (!ts->attributes[i]->entries[j])
				continue; /* empty pigeonhole */
			printf("  class %s:\n",
     			       ts->attributes[i]->entries[j]->key);
			ce = (struct class_entry*)
				ts->attributes[i]->entries[j]->data;
			for (k = 0; k < ce->refmap->size; k++) {
				if (!ce->refmap->entries[k])
					continue; /* empty pigeonhole */
				printf("        refmap %s ->\t%d\n",
				       ce->refmap->entries[k]->key,
				       (size_t) ce->refmap->entries[k]->data);
			}

		}

	}

	printf("classes: %u\n", ts->refclasses->size);
	for (i = 0; i < ts->refclasses->size; i++) {
		if (!ts->refclasses->entries[i])
			continue;
		printf("class: %s, count: %u\n",
		       ts->refclasses->entries[i]->key,
		       (unsigned int) ts->refclasses->entries[i]->data);
	}

}

struct table_stats *parse_line(struct table_stats *ts, char *line,
		size_t line_size, size_t refattr)
{
	char *refclass;
	char *token;
	char *last;
	size_t refsize;
	size_t ia;
	size_t nrefclasses;
	size_t size;
	unsigned int refcount;
	unsigned int refhash;
	struct class_entry *ce;
	struct hash_table *classes;

	/* first of all obtain the hash of the reference
	 * attribute that is used in this line, this way we
	 * can store the stats for each attribute class in
	 * a simple loop around strtok_r */
	refclass = strrchr(line, ',') + 1;
	if (!refclass) {
		fprintf(stderr, "ouch, missing comma in line: "
				"%s\n", line);
		goto failed;
	}
	refsize = strnlen(refclass, line_size);
	refhash = get_hash(refclass, refsize);

	refcount  = (unsigned int)
		hash_get(ts->refclasses, refclass, refsize, refhash);
	hash_put(ts->refclasses, refclass, refsize,
	         (void*)(refcount + 1), refhash);

	/* now we can iterate over the classes in this line
	 * and update their stats */
	for (ia = 0; token = strtok_r(line, ",", &last);
	     ia++, line = NULL) {
		if (ia == refattr)
			continue;

		classes = ts->attributes[ia];
		size = strnlen(token, line_size);
		ce = (struct class_entry*)
		        hash_get(classes, token, size, 0);
		if (!ce) {
			ce = new_class_entry();
			if(!ce)
				goto failed;
			if(!hash_put(classes, token, size, (void*)ce, 0))
				goto failed;
		}

		ce->count++;

		/* increase the count for this class in
		 * this refclass */
		nrefclasses = (size_t) hash_get(ce->refmap, refclass,
		                                refsize, refhash);
		hash_put(ce->refmap, refclass, refsize,
		         (void*) (nrefclasses + 1), refhash);
	}
	return ts;
failed:
	return NULL;
}

void strip_newline(char *str)
{
	for (;*str; str++)
		if (*str == '\n' || *str == '\r')
			*str = '\0';
}

struct table_stats *collect_stats(FILE *stream)
{
	char line[BUFSIZ];
	char *lineptr; /* used by strsep */
	unsigned int refhash;
	size_t nr_attributes;
	size_t refattr;
	struct table_stats *ts = NULL;

	nr_attributes = 0;
	while (fgets(line, sizeof(line)-1, stream)) {
		lineptr = line;

		strip_newline(line);

		if (nr_attributes == 0) {
			/* count how many attributes we have and initialize
			 * the needed structures */
			while (*lineptr++)
				if (*lineptr == ',')
					nr_attributes++;
			nr_attributes++;
			refattr = nr_attributes - 1;

			ts = new_table_stats(nr_attributes);
			if (!ts)
				goto failed;

			fseek(stream, 0L, SEEK_SET);
		}
		else {
			if (!parse_line(ts, line, sizeof(line)-1, refattr))
				goto failed;
			ts->lines++;
		}
	}
	if (!feof(stream))
		goto failed;

	return ts;
failed:
	free_table_stats(ts);
	return NULL;

}

int *sort_by_gain() {
}

int main(int argc, char **argv) 
{
	size_t i;
	FILE *stream;
	struct table_stats *ts;

	for (i = 1; i < argc; i++) {
		stream = fopen(argv[i], "r");
		if (!stream) {
			perror("opening file");
			return 1;
		}

		ts = collect_stats(stream);
		dump_table_stats(ts);
		if (!ts) {
			perror("parsing file");
			return 1;
		}
	}

	return 0;
}
