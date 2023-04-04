#ifndef B_LIST_H
#define B_LIST_H

#include <stdlib.h>

#define DEFAULT_CAP 1
#define EXPAND_FACTOR 2

struct list {
	void **elems;
	size_t e_size;
	size_t cap;
	size_t size;
};

struct list *list_init(size_t e_size);
int list_add(struct list *list, void *elem);
void *list_remove(struct list *list, size_t idx);
void *list_removelast(struct list *list);
void *list_get(struct list *list, size_t idx);

#endif
