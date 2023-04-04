#include "list.h"

struct list *list_init(size_t e_size) {
	struct list *l = malloc(sizeof(struct list));
	if (l) {
		l->e_size = e_size;
		l->cap = DEFAULT_CAP;
		l->size = 0;
		l->elems = calloc(l->cap, sizeof(void *));
		if (!l->elems) {
			free(l);
			l = NULL;
		}
	}

	return l;
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

	list->elems[list->size++] = elem;

	return 0;
}

void *list_remove(struct list *list, size_t idx) {
	if (idx >= list->size) {
		return NULL;
	}

	void *ret = list->elems[idx];
	
	for (size_t i = idx; i < list->size - 1; i++) {
		list->elems[i] = list->elems[i + 1];
	}

	list->elems[list->size - 1] = NULL;
	list->size--;

	return ret;
}

void *list_removelast(struct list *list) {
	return list_remove(list, list->size);
}

void *list_get(struct list *list, size_t idx) {
	if (idx >= list->size) {
		return NULL;
	}

	return list->elems[idx];
}
