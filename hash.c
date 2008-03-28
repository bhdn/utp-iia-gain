#include <stdlib.h>
#include <string.h>

#include "hash.h"

void *hash_get(struct hash_table *table, unsigned char *key, size_t
		key_len, unsigned int force_hash)
{
	unsigned int hash;
	size_t pos;
	struct hash_entry *found;
	void *data = NULL;

	hash = force_hash ? force_hash : get_hash(key, key_len);
	pos = hash % table->size;
	found = table->entries[pos];
	if (found && found->next) {
		/* ouch! */
		while (found) {
			if (strncmp(key, found->key, key_len) == 0) {
				data = found->data;
				break;
			}
			found = found->next;
		}
	}
	else if (found)
		data = found->data;

	return data;
}

void *hash_put(struct hash_table *table, unsigned char *key, size_t
		key_len, void *data, unsigned int force_hash)
{
	unsigned int hash;
	size_t pos;
	struct hash_entry *found, *new;

	new = (struct hash_entry*) malloc(sizeof(struct hash_entry));
	if (!new)
		/* shhhhhhhh */
		return NULL;
	new->next = NULL;
	new->key = (char*) malloc(key_len + 1);
	if (!new->key) {
		free(new);
		return NULL;
	}
	strncpy(new->key, key, key_len);
	new->key_len = key_len;
	new->data = data;
	hash = force_hash ? force_hash : get_hash(key, key_len);
	pos = hash % table->size;
	found = table->entries[pos];
	if (!found)
		table->entries[pos] = new;
	else {
		while (found->next)
			found = found->next;
		found->next = new;
	}

	return new; /* just to say it didn't fail */
}

void hash_free_entry(struct hash_entry *entry)
{
	struct hash_entry *next;

	while (entry) {
		next = entry->next;
		free(entry->key);
		free(entry);
		entry = next;
	}
}

void hash_free(struct hash_table *table)
{
	size_t i;
	/* free the entries */
	for (i = 0; i < table->size; i++)
		if (table->entries[i])
			hash_free_entry(table->entries[i]);
}

struct hash_table *hash_init(size_t size)
{
	size_t i, toalloc;
	struct hash_table *table;

	table = (struct hash_table*) malloc(sizeof(struct hash_table));
	if (!table)
		return NULL;
	toalloc = sizeof(struct hash_entry*) * size;
	table->entries = (struct hash_entry**) malloc(toalloc);
	if (!table->entries) {
		free(table);
		return NULL;
	}
	table->size = size;
	memset((void*)table->entries, 0, toalloc);

	return table;
}

/* from http://www.burtleburtle.net/bob/hash/doobs.html */
unsigned int get_hash(unsigned char *key, size_t key_len)
{
    unsigned int hash = 0;
    size_t i;
 
    for (i = 0; i < key_len; i++){
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

