#include <stdio.h>

#include "bencode.h"
#include "util.h"
#include "bt_client.h"
#include "byte_str.h"

int main() {
	struct bt_metainfo m = {0};
	struct bt_metainfo *metainfo = &m;

	char *path = "./testing/test_torrents/ubuntu-22.10-desktop-amd64.iso.torrent";

	populate_torrent_metainfo(metainfo, path);
	print_torrent_metainfo(metainfo);

	printf("uid: [%s]\n", generate_id());

	char *buf = "\x12\x34\x56\x78\x9a\xbc\xde\xf1\x23\x45\x67\x89\xab\xcd\xef\x12\x34\x56\x78\x9a";

	printf("encode: [%s]\n", HTTP_encode_buffer((uint8_t *) buf, 20));
	puts("expect: [%124Vx%9A%BC%DE%F1%23Eg%89%AB%CD%EF%124Vx%9A]\n");

	struct bt_instance b = {0};
	struct bt_instance *instance = &b;
	instance->client_id = generate_id();
	instance->t_metainfo = metainfo;

	send_tracker_request(instance);


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
	//printf("bytestr: [%s]\n", bs->ptr);
}
