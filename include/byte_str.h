#ifndef BYTESTR_H
#define BYTESTR_H

#include <stdlib.h>
#include <string.h>

struct bytestr {
	char *ptr;  // string ptr
	size_t len; // length of str not including '\0`
	size_t cap; // capacityof str not including '\0'
};

void bytestr_init(struct bytestr *str);
void bytestr_append(struct bytestr *str, char *s);
struct bytestr *bytestr_get(char *str, size_t len);

#endif
