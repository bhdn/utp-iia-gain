#ifndef inc_hash_h

struct hash_entry {
	unsigned char *key;
	void *data;
	struct hash_entry *next;
};

struct hash_table {
	size_t size;
	hash_entry *entries;
};

struct hash_table *hash_init(size_t);
void *hash_put(struct hash_table *, unsigned char *, size_t, void *);
void *hash_get(struct hash_table *, unsigned char *, size_t);
void hash_free(struct hash_table *);

#endif /* ifndef inc_hash_h */
