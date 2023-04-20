#ifndef B_LIST_H
#define B_LIST_H

#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAP 1
#define EXPAND_FACTOR 2

struct list {
	void *elems;
	size_t e_size;
	size_t cap;
	size_t size;
};

struct list * list_init(size_t e_size);
void          list_free(struct list *list);
int           list_add(struct list *list, void *elem);
void *        list_remove(struct list *list, size_t idx);
void *        list_removelast(struct list *list);
void *        list_get(struct list *list, size_t idx);

// XXX: if called twice, _elem must be different name or redefine error
#define FOREACH_LIST_ELEM(_elem, _idx, _list) \
	void *_elem = _list->elems;\
	for(unsigned int _idx = 0; \
			_idx < _list->size; \
			++idx, _elem = (char *) _list->elems + (_idx * _list->e_size))
#endif
