/**
 *
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
struct class_list {
	unsigned long count;
	struct hash_table *refmap; /* counts in which classes of the ref.
				      attribute it appeared */
	class_list *next;
};

struct table_stats {
	unsigned long lines;
	struct class_list *attributes;
	size_t nr_attributes;
};

struct class_list *new_class_list(const char *name)
{
	struct class_list *cl;

	cl = (struct class_list*) malloc(sizeof(class_list));
	if (!cl)
		return NULL;
	cl->hash = hash(name);
	cl->next = NULL;

	return cl;	
}

void free_class_list(struct class_list *cl)
{
	struct class_list *next;
	while (cl) {
		next = cl->next;
		hash_free(cl->refmap);
		free(cl);
		cl = next;
	}
}

struct table_stats *new_table_stats(void)
{
	struct table_stats *ts;

	ts = (struct table_stats*) malloc(sizeof(struct table_stats));
	if (!ts)
		return NULL;
	ts->lines = 0;
	ts->attributes = NULL;
	ts->nr_attributes = 0;
}

void free_table_stats(struct table_stats *ts)
{
	if (ts->attributes)
		free_class_list(ts->attributes);
	free(ts);
}

int count_attributes(FILE *stream)
{
}

struct table_stats *collect_stats(FILE *stream)
{
	struct table_stats *ts;
	char line[MAXBUF];
	char *lineptr, *token; /* used by strsep */
	int nr_attributes;
	int first_line = 1;

	ts = new_table_stats();
	if (!ts)
		return NULL;

	while (1) {
		if (!fgets(line, sizeof(line) - 1, stream)) {
			if (!feof(stream)) {
				free_table_stats(ts);
				return NULL;
			}
			break;
		}
		lineptr = line;
		nr_attributes = 0;
		while (token = strsep(&lineptr, ",")) {
			nr_attributes++;
		}
	}

}

int *sort_by_gain() {
}

int main(int argc, char **argv) 
{
	//
}
