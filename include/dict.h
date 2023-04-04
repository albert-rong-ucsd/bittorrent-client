#ifndef B_DICT_H
#define B_DICT_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DEFAULT_CAP 2
#define LOAD_FACTOR 0.75
#define EXPAND_FACTOR 2

struct dict_elem {
	struct dict_elem *next;
	size_t size;
	char *key;
	char val[];
};

struct dict {
	struct dict_elem **elems;
	size_t cap;
	size_t size;
};

struct dict *dict_init();
int dict_add(struct dict *dict, char *key, unsigned char *val, int val_len);
int dict_remove(struct dict *dict, char *key);
void *dict_get(struct dict *dict, char *key);

#endif
