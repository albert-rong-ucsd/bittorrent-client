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
int dict_add(struct dict *dict, char *key, void *val, int val_len);
int dict_remove(struct dict *dict, char *key);
void *dict_get(struct dict *dict, char *key);

#define FOREACH_DICT_ELEM(_elem, _dict) \
	unsigned int macro_idx = 0; \
	for (struct dict_elem *e = _dict->elems[macro_idx]; \
			macro_idx < _dict->cap; \
			e = _dict->elems[++macro_idx]) \
		for (struct dict_elem *_elem = e, *next = (e) ? e->next : NULL; \
				_elem; \
				_elem = next, next = (next) ? next->next : NULL)

#endif
