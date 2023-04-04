#include "byte_str.h"

void bytestr_init(struct bytestr *str) {
	str->ptr = malloc(11);
	str->len = 0;
	str->cap = 10;

	str->ptr[0] = '\0';
}

void bytestr_append(struct bytestr *str, char *s) {
	size_t len = strlen(s);

	while (len + str->len > str->cap) {
		size_t new_cap = str->cap * 2 + 1;
		str->ptr = realloc(str->ptr, new_cap);
		if (str->ptr == NULL) {
			exit(1); // bruh you can't realloc
		}

		str->cap = new_cap;
	}

	strcpy(str->ptr + str->len, s);
	str->len += len;
}

struct bytestr *bytestr_get(char *str, size_t len) {
	struct bytestr *s = malloc(sizeof(struct bytestr));
	s->ptr = malloc(len + 1);
	memcpy(s->ptr, str, len);
	s->ptr[len] = '\0';
	s->len = len;
	s->cap = len + 1;
	return s;
}
