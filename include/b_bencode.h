#ifndef B_BENCODE_H
#define B_BENCODE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "byte_str.h"
#include "list.h"
#include "dict.h"

enum B_TYPE {
	INT,
	STR,
	LIST,
	DICT
};

struct ben_node {
	enum B_TYPE type;
	union {
		int64_t i;
		struct bytestr *s;
		struct list *l;
		struct dict *d;
	};
};

struct ben_node *bencode_decode_str(char *buf, char **end);
struct ben_node *bencode_decode_file(char *filename);
void bencode_print(struct ben_node *node);

#endif
