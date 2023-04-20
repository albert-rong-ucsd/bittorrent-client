#include "list.h"
#include <stdio.h>


struct list *list_init(size_t e_size) {
	struct list *l = malloc(sizeof(struct list));
	if (l) {
		l->e_size = e_size;
		l->cap = DEFAULT_CAP;
		l->size = 0;
		l->elems = calloc(l->cap, e_size);
		if (!l->elems) {
			free(l);
			l = NULL;
		}
	}

	return l;
}

void list_free(struct list *list) {
	free(list->elems);
	free(list);
}

int list_add(struct list *list, void *elem) {
	if (list->size == list->cap) {
		size_t new_cap = list->cap * EXPAND_FACTOR;
		void *new_list = realloc(list->elems, new_cap * list->e_size);

		if (!new_list) {
			return 1;
		}

		list->elems = new_list;
		list->cap = new_cap;
	}

	memcpy((char *) list->elems + (list->size * list->e_size), elem, list->e_size);
	list->size++;
	return 0;
}

// returns ptr to prev val, must be freed?
void *list_remove(struct list *list, size_t idx) {
	if (idx >= list->size) {
		return NULL;
	}

	void *removed = malloc(list->e_size);
	memcpy(removed, (char *) list->elems + (idx * list->e_size), list->e_size);
	
	for (size_t i = idx; i < list->size - 1; i++) {
		memcpy((char *) list->elems + (i * list->e_size),
				(char *) list->elems + ((i + 1) * list->e_size),
				list->e_size);
	}

	list->size--;
	return removed;
}

void *list_removelast(struct list *list) {
	return list_remove(list, list->size);
}

void *list_get(struct list *list, size_t idx) {
	if (idx >= list->size) {
		return NULL;
	}

	return (char *) list->elems + (idx * list->e_size);
}
