#include <stdio.h>

#include "b_bencode.h"
#include "util.h"
#include "bt_client.h"
#include "byte_str.h"
#include "list.h"
#include "dict.h"

void list_test();
void list_printINT(struct list *list);
void dict_test();
void dict_printall(struct dict *dict);
void ben_test();
void ben_test2();
void randomtest();

int main() {
	randomtest();
}

void ben_test2() {
	char *path = "./testing/test_torrents/ubuntu-22.10-desktop-amd64.iso.torrent";

	struct ben_node *n = bencode_decode_file(path);
	bencode_print(n);
}

void ben_test() {
	char *benstr = "d3:cow3:moo4:spam4:eggse";
	char *benstr2 = "l5:helloi42ee";

	char *end;
	struct ben_node *n = bencode_decode_str(benstr2, &end);
	bencode_print(n); }

void dict_printall(struct dict *dict) {
	printf("DICT:\n");
	printf("size: [%lu], cap: [%lu] {\n", dict->size, dict->cap);
	FOREACH_DICT_ELEM(elem, dict) {
		printf("	key: [%s], val: [", elem->key);
		int valsize = elem->size - sizeof(struct dict_elem);
		switch(valsize) {
			case sizeof(char):
				printf("%c", elem->val[0]);
				break;
			case sizeof(int):
				printf("%d", *((int *)elem->val));
				break;
			default:
				printf("%s", elem->val);
				break;
		}
		printf("]\n");
	}
	printf("}\n");

}

void dict_test() {
	struct dict *d = dict_init();
	dict_printall(d);

	char *key1 = "abcdef";
	char *key2 = "zzztoj";
	char *key3 = "c";

	int val1 = 123456;
	char *val2 = "jaksldfjklsafljasdf;skdfjasldfjkls";
	char val3 = 'a';

	dict_add(d, key1, &val1, sizeof(val1));
	dict_add(d, key2, val2, strlen(val2) + 1);
	dict_add(d, key3, &val3, sizeof(val3));
	dict_printall(d);
}

void list_printINT(struct list *list) {
	printf("LIST:\n");
	printf("size: [%lu], cap: [%lu], e_size: [%lu]\n", list->size, list->cap, list->e_size);
	FOREACH_LIST_ELEM(e, idx, list) {
		printf("idx: [%d], ptr: [%p], data: [%d]\n", idx, (void *) e, *((int *) e));
	}
}


void list_test() {
	struct list *l = list_init(sizeof(int));
	int a = 0;
	int b = 1;
	int c = 10;
	list_add(l, &a);
	list_add(l, &b);
	list_add(l, &c);

	list_printINT(l);

	int *removed = list_remove(l, 1);
	printf("removed: [%p], val: [%d]\n", (void *)removed, *removed);
	list_printINT(l);
}

void idktest() {
	printf("uid: [%s]\n", generate_id());

	char *buf = "\x12\x34\x56\x78\x9a\xbc\xde\xf1\x23\x45\x67\x89\xab\xcd\xef\x12\x34\x56\x78\x9a";

	printf("encode: [%s]\n", HTTP_encode_buffer((uint8_t *) buf, 20));
	puts("expect: [%124Vx%9A%BC%DE%F1%23Eg%89%AB%CD%EF%124Vx%9A]\n");
}

void bytestrtest() {
	//printf("==================");
	//struct bytestr b = { 0 };
	//struct bytestr *bs = &b;
	//bytestr_init(bs);

	//printf("bytestr: [%s]\n", bs->ptr);
	//bytestr_append(bs, "asdsafd");
	//printf("bytestr: [%s]\n", bs->ptr);
	//bytestr_append(bs, "BRUH");
	//printf("bytestr: [%s]\n", bs->ptr);
	//bytestr_append(bs, "BRUHBRUH");
	//printf("bytestr: [%s]\n", bs->ptr);
	//bytestr_append(bs, "BRUHBRUHHAHAHAHSKAHFJLKAJQOWEIQWOEJIQWEOQEWJOIQJOWIEQWOJIEhellomario");
}

void randomtest() {
	struct bt_metainfo m = {0};
	struct bt_metainfo *metainfo = &m;

	char *path = "./testing/test_torrents/ubuntu-22.10-desktop-amd64.iso.torrent";

	populate_torrent_metainfo(metainfo, path);
	print_torrent_metainfo(metainfo);


	struct bt_instance b = {0};
	struct bt_instance *instance = &b;
	instance->client_id = generate_id();
	instance->t_metainfo = metainfo;

	send_tracker_request(instance);
}
