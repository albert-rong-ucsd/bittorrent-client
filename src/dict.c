#include "dict.h"

struct dict *dict_init() {
	struct dict *d = malloc(sizeof(struct dict));

	if (d) {
		d->cap = DEFAULT_CAP;
		d->size = 0;
		d->elems = calloc(d->cap, sizeof(struct dict_elem *));

		if (!d->elems) {
			free(d);
			d = NULL;
		}
	}

	return d;
}

struct dict_elem *dict_elem_init(char *key, unsigned char *val, size_t val_len) {
	struct dict_elem *elem = malloc(sizeof(struct dict_elem) + val_len);
	size_t key_size = strlen(key) + 1;
	elem->key = malloc(key_size);
	memcpy(elem->key, key, key_size);
	memcpy(elem->val, val, val_len);
	elem->next = NULL;
	elem->size = sizeof(struct dict_elem) + val_len;
	
	return elem;
}

// Jenkins OAAT hash
uint32_t hash(char *key) {
	uint32_t hash = 0;
	while (*key) {
		hash += *(key++);
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

int ensure_load_factor(struct dict *dict) {
	if (dict->size >= dict->cap * LOAD_FACTOR) {
		size_t new_cap = dict->cap * EXPAND_FACTOR;
		struct dict_elem **resized = calloc(new_cap, sizeof(struct dict_elem *));

		if (resized) {
			for (unsigned int i = 0; i < dict->cap; i++) {
				if (dict->elems[i] != NULL) {
					for (struct dict_elem *curr = dict->elems[i]; curr; curr = curr->next) {
						
					}
				}
			}
			return 0;
		}
	}

	return 1;
}

int dict_add(struct dict *dict, char *key, unsigned char *val, int val_len) {
	uint32_t add_idx = hash(key) % dict->cap;

	struct dict_elem *prev = NULL;
	struct dict_elem *curr = dict->elems[add_idx];
	while (curr && strcmp(curr->key, key)) {
		prev = curr;
		curr = curr->next;
	}

	if (curr) {
		curr->size = sizeof(struct dict_elem) + val_len;
		memcpy(curr->val, val, val_len);

		return 1;
	}

	struct dict_elem *add = dict_elem_init(key, val, val_len);

	if (prev) {
		prev->next = add;
	}
	else {
		dict->elems[add_idx] = add;
	}

	dict->size++;

	return 0;
}

int dict_remove(struct dict *dict, char *key) {
	uint32_t rm_idx = hash(key) % dict->cap;

	struct dict_elem *prev = NULL;
	struct dict_elem *curr = dict->elems[rm_idx];
	while (curr && strcmp(curr->key, key)) {
		prev = curr;
		curr = curr->next;
	}

	if (!curr) {
		return 1;
	}

	if (!prev) {
		dict->elems[rm_idx] = NULL;
	}
	else {
		prev->next = curr->next;
	}

	dict->size--;
	free(curr->key);
	free(curr);

	return 0;
}

void *dict_get(struct dict *dict, char *key) {
	uint32_t get_idx = hash(key) % dict->cap;

	struct dict_elem *curr = dict->elems[get_idx];
	while (curr && strcmp(curr->key, key)) {
		curr = curr->next;
	}

	return (curr) ? (void *) curr->val : curr; // TODO: double check
}
