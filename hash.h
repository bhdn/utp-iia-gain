#ifndef inc_hash_h

struct hash_entry {
	unsigned char *key;
	size_t key_len;
	void *data;
	struct hash_entry *next;
};

struct hash_table {
	size_t size;
	struct hash_entry **entries;
};

struct hash_table *hash_init(size_t);
void *hash_put(struct hash_table *, unsigned char *, size_t, void *,
		unsigned int);
void *hash_get(struct hash_table *, unsigned char *, size_t, unsigned int);
void hash_free(struct hash_table *);
unsigned int get_hash(unsigned char *, size_t);

#endif /* ifndef inc_hash_h */
