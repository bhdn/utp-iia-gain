#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned int hash_t;

struct class_list {
	hash_t hash;
	class_list *next;
};

struct attribute {
	class_list *classes;
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
		free(cl);
		cl = next;
	}
}

int main(int argc, char **argv) 
{
	//
}
