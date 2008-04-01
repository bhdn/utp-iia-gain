#ifndef inc_hash_h

struct hash_entry {
#ifdef DEBUG
	char *key;
#endif
	size_t key_len;
	unsigned int hash;
	void *data;
	struct hash_entry *next;
};

struct hash_table {
    size_t count; /* the number of real valid entries */
	size_t size; /* the number of entries allocated */
	struct hash_entry **entries;
};

/* opaque iterator state */
typedef struct hash_iter_t_ {
    size_t i;
    struct hash_entry *current;
}hash_iter_t;

struct hash_table *hash_init(size_t);
void *hash_put(struct hash_table *, unsigned char *, size_t, void *,
		unsigned int);
void *hash_get(struct hash_table *, unsigned char *, size_t, unsigned int);
void hash_free(struct hash_table *);
unsigned int get_hash(unsigned char *, size_t);

hash_iter_t hash_iter_first(struct hash_table *, void **);
hash_iter_t hash_iter_next(struct hash_table *, hash_iter_t, void **);
int hash_iter_done(struct hash_table *, hash_iter_t);

/* hide the crap: */

#define for_each_hash_value(table, iter, data) \
	for (iter = hash_iter_first(table, (void**)&data); \
	     hash_iter_done(table, iter); \
	     iter = hash_iter_next(table, iter, (void**)&data))

#endif /* ifndef inc_hash_h */
