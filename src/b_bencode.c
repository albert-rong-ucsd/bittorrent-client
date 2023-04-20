#include "b_bencode.h"

#include <sys/stat.h>

int get_bentype(char c);
struct ben_node *bencode_decode_str(char *buf, char **end);
struct ben_node *decode_int(char *buf, char **end);
struct ben_node *decode_str(char *buf, char **end);
struct ben_node *decode_list(char *buf, char **end);
struct ben_node *decode_dict(char *buf, char **end);

char *read_file(char *file, long long *len) {
	struct stat s;
	if (stat(file, &s)) {
		return NULL;
	}
	*len = s.st_size;

	FILE *f = fopen(file, "r");
	if (!f) {
		return NULL;
	}

	char *read = malloc(*len);
	fread(read, 1, *len, f);
	fclose(f);
	return read;
}

struct ben_node *bencode_decode_file(char *filename) {
	char *tor_str;
	long long tor_size;
	tor_str = read_file(filename, &tor_size);

	char *end;
	struct ben_node *node = bencode_decode_str(tor_str, &end);
	// TODO: maybe ensure end - tor_str ~ tor_size?
	free(tor_str);
	return node;
}

struct ben_node *bencode_decode_str(char *buf, char **end) {
	switch (get_bentype(*buf)) {
		case INT:
			return decode_int(buf, end);
		case STR:
			return decode_str(buf, end);
		case LIST:
			return decode_list(buf, end);
		case DICT:
			return decode_dict(buf, end);
		default:
			return NULL;
	}
}

int get_bentype(char c) {
	if (c == 'i') {
		return INT;
	}
	else if (c == 'l') {
		return LIST;
	}
	else if (c == 'd') {
		return DICT;
	}
	else if (isdigit(c)) {
		return STR;
	}
	else {
		return -1;
	}
}

/**
 * handle:
 * 		- ie
 * 		- int too large
 */
struct ben_node *decode_int(char *buf, char **end) {
	if (*buf != 'i') {
		return NULL;
	}

	unsigned int curr_off = (*(buf + 1) == '-') ? 2 : 1;
	char curr;
	while (isdigit(curr = *(buf + curr_off))) {
		curr_off++;
	}

	if (curr != 'e') {
		return NULL;
	}

	struct ben_node *node = malloc(sizeof(struct ben_node));
	node->type = INT;
	node->i = atoll((char *) buf + 1);
	*end = buf + curr_off + 1;

	return node;
}

struct ben_node *decode_str(char *buf, char **end) {
	char *curr = buf;

	while (*curr != ':') {
		if (!isdigit(*curr)) {
			return NULL;
		}
		curr++;
	}
	int len = atoi((char *) buf);

	struct ben_node *node = malloc(sizeof(struct ben_node));
	node->type = STR;
	node->s = bytestr_get(curr + 1, len);
	*end = curr + len + 1;

	return node;
}

struct ben_node *decode_list(char *buf, char **end) {
	assert(*buf == 'l');
	struct list *l = list_init(sizeof(struct ben_node));

	char *curr = buf + 1;
	while (*curr != 'e') {
		char *curr_end;
		list_add(l, bencode_decode_str(curr, &curr_end));
		curr = curr_end;
	}
	
	struct ben_node *node = malloc(sizeof(struct ben_node));
	node->type = LIST;
	node->l = l;
	*end = curr + 1;

	return node;
}

/**
 * safety on dict key?
 */
struct ben_node *decode_dict(char *buf, char **end) {
	assert(*buf == 'd');
	struct dict *d = dict_init();

	char *curr = buf + 1;
	while (*curr != 'e') {
		char *val_buf, *next_curr;
		struct ben_node *key_node = decode_str(curr, &val_buf);
		struct ben_node *val_node = bencode_decode_str(val_buf, &next_curr);
		dict_add(d, key_node->s->ptr, (unsigned char *) val_node, sizeof(struct ben_node));
		curr = next_curr;

		free(key_node->s);
		free(key_node);
	}

	struct ben_node *node = malloc(sizeof(struct ben_node));
	node->type = DICT;
	node->d = d;
	*end = curr + 1;

	return node;
}

void h_bencode_pindent(unsigned int indent) {
	while (indent--) {
		printf("\t");
	}
}

void h_bencode_print(struct ben_node *node, unsigned int depth) {
	h_bencode_pindent(depth);
	switch (node->type) {
		case INT:
			printf("int = %lld\n", (long long) node->i);
			break;
		case STR:
			printf("str = ");
			fwrite(node->s->ptr, 1, node->s->len, stdout);
			printf(" (len = %lu)\n", node->s->len);
			break;
		case LIST:
			printf("list = [\n");
			FOREACH_LIST_ELEM(elem, idx, node->l) {
				h_bencode_print(elem, depth + 1);
			}
			h_bencode_pindent(depth);
			printf("(size: %lu) ]\n", node->l->size);
			break;
		case DICT:
			printf("dict = {\n");
			FOREACH_DICT_ELEM(elem, node->d) {
				h_bencode_pindent(depth+1);
				printf("key: %s\n", elem->key);
				h_bencode_print((struct ben_node *)elem->val, depth + 1);
				printf("\n");
			}
			h_bencode_pindent(depth);
			printf("(size: %lu) }\n", node->d->size);
			break;
		default:
			printf("ERROR: invalid node encountered\n");
			exit(1);
			break;
	}
}

void bencode_print(struct ben_node *node) {
	h_bencode_print(node, 0);
}
