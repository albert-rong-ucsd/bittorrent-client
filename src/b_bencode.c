#include "b_bencode.h"

int get_bentype(char c);
struct ben_node *decode_int(char *buf, char **end);
struct ben_node *decode_str(char *buf, char **end);
struct ben_node *decode_list(char *buf, char **end);
struct ben_node *decode_dict(char *buf, char **end);

struct ben_node *bencode_decode(char *buf, char **end) {
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

	char *curr = buf;
	while (*curr != 'e') {
		char *curr_end;
		list_add(l, bencode_decode(curr, &curr_end));
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

	char *curr = buf;
	while (*curr != 'e') {
		char *val_buf, *next_curr;
		struct ben_node *key_node = decode_str(curr, &val_buf);
		struct ben_node *val_node = bencode_decode(val_buf, &next_curr);
		dict_add(d, key_node->s->ptr, (unsigned char *) val_node, sizeof(struct ben_node *));
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
			// TODO: iterate depth + 1
			h_bencode_pindent(depth);
			printf("]\n");
			break;
		case DICT:
			printf("dict = {\n");
			// TODO: iterate depth + 1
			h_bencode_pindent(depth);
			printf("dict = {\n");
			break;
		default:
			printf("bruh this aint a node, see ya\n");
			exit(1);
			break;
	}
}

void bencode_print(struct ben_node *node) {
	h_bencode_print(node, 0);
}
