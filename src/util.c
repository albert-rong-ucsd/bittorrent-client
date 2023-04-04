#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <openssl/sha.h>

#include "sha1.h"
#include "util.h"
#include "bencode.h"
#include "byte_str.h"
#include "tracker_announce.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void copy_str(char **dest, struct be_node *src_node) {
	if (src_node == NULL) {
		*dest = NULL;
		return;
	}

	char *to_copy = src_node->val.s;
	size_t len = be_str_len(src_node);
	*dest = malloc(len + 1);
	memcpy(*dest, to_copy, len);
	(*dest)[len] = '\0';
}

uint8_t *calculate_infohash(char *filepath) {
	struct bytestr *info = get_infoval(filepath);

	uint8_t *hash = malloc(20);

	SHA1((unsigned char *) info->ptr, info->len, hash);

	free(info->ptr);
	free(info);
	
	return hash;
}

void populate_torrent_metainfo(struct bt_metainfo *metainfo, char *filepath) {
	struct be_node *node = load_be_node(filepath);

	be_dump(node);

	if (node->type != BE_DICT) { // TODO: return error code or something?
		return;
	}

	// fill in metainfo
	copy_str(&metainfo->announce, be_dict_lookup(node, "announce"));
	copy_str(&metainfo->comment, be_dict_lookup(node, "comment"));
	copy_str(&metainfo->creator, be_dict_lookup(node, "created by"));
	metainfo->created = malloc(sizeof(time_t));
	*metainfo->created = *be_dict_lookup_num(node, "creation date"); // TODO: BRUH THIS WILL NPTREXCEPT

	// fill in info
	struct be_node *info_dict = be_dict_lookup(node, "info");
	metainfo->info = malloc(sizeof(struct bt_info));
	struct bt_info *info = metainfo->info;

	copy_str(&info->name, be_dict_lookup(info_dict, "name"));
	info->piece_length = *be_dict_lookup_num(info_dict, "piece length");

	struct be_node *pieces_hash_node = be_dict_lookup(info_dict, "pieces");
	long long pieces_len = be_str_len(pieces_hash_node);
	info->sha_hashes = malloc(pieces_len);
	memcpy(info->sha_hashes, pieces_hash_node->val.s, pieces_len);

	info->num_pieces = pieces_len / 20;
	info->info_hash = calculate_infohash(filepath);

	// fill in fileinfo
	long long *file_len = be_dict_lookup_num(info_dict, "length");
	if (file_len != NULL) { // single file mode
		info->num_files = 1;

		info->f_info = malloc(sizeof(struct bt_fileinfo));
		info->f_info->name = info->name;
		info->f_info->length = *file_len;
		info->f_info->path = malloc(2);
		memcpy(info->f_info->path, ".", 2);
	}
	else { // multifile mode
		struct be_node *files_node = be_dict_lookup(info_dict, "files");

		size_t i;
		for (i = 0; files_node->val.l[i]; i++) { }
		info->num_files = i;
		
		info->f_info = malloc(sizeof(struct bt_fileinfo) * info->num_files);
		for (i = 0; files_node->val.l[i]; i++) {
			struct be_node *file_dict = files_node->val.l[i];
			struct bt_fileinfo *curr_file = &info->f_info[i];

			copy_str(&curr_file->name, be_dict_lookup(file_dict, "name"));
			curr_file->length = *be_dict_lookup_num(file_dict, "length");
			// copy_str(&curr_file->path, be_dict_lookup(file_dict, "path")); TODO: FIX LINE NOT A STRING
		}
	}
}

static inline void print_fileinfo(struct bt_fileinfo *fileinfo) {
	printf("		file: {\n");
	printf("			name: [%s]\n", fileinfo->name);
	printf("			length: %llu\n", fileinfo->length);
	printf("			path: [%s]\n", fileinfo->path);
	printf("		}\n");
}

static inline void print_info(struct bt_info *info) {
	printf("	info : {\n");
	printf("		number of files: %d\n", info->num_files);
	printf("		piece length: %d\n", info->piece_length);
	printf("		num_pieces: %d\n", info->num_pieces);
	for (int i = 0; i < info->num_files; i++) {
		print_fileinfo(&info->f_info[i]);
	}
	printf("	}\n");
}

void print_torrent_metainfo(struct bt_metainfo *metainfo) {
	printf("torrent information : {\n");
	printf("	announce: [%s]\n", metainfo->announce);
    printf("	creation time: %s", (metainfo->created) ? asctime(gmtime(metainfo->created)) : "(null)\n");
	printf("	comment: [%s]\n", metainfo->comment);
	printf("	creator: [%s]\n", metainfo->creator);
	printf("	encoding: [%s]\n", metainfo->encoding);
	print_info(metainfo->info);
	printf("}\n");
}



bool is_readable(char c) {
	return isalnum(c) || c == '.' || c == '-' || c == '_' || c == '~';
}

char *HTTP_encode_buffer(uint8_t *byte_str, size_t len) {
	size_t len_encoded = len;
	for (unsigned int i = 0; i < len; i++) {
		if (!is_readable(*(byte_str + i))) {
			len_encoded += 2;
		}
	}

	char *encoded = malloc(len_encoded + 1);
	char *curr_ptr = encoded;
	for (unsigned int i = 0; i < len; i++) {
		uint8_t curr = *(byte_str + i);
		if (!is_readable(curr)) {
			memcpy(curr_ptr, "%", 1);
			sprintf(curr_ptr + 1, "%02X", curr);
			curr_ptr += 3;
		}
		else {
			memcpy(curr_ptr, &curr, 1);
			curr_ptr += 1;
		}
	}

	memcpy(curr_ptr, "\0", 1);
	return encoded;
}

static inline void append_param(struct bytestr *bs, char *param_name, char *val) {
	bytestr_append(bs, param_name);
	bytestr_append(bs, "=");
	bytestr_append(bs, val);
	bytestr_append(bs, "&");
}

char *get_HTTP_tracker_request(struct tracker_req *tr) {
	struct bytestr b = {0};
	struct bytestr *bs = &b;
	bytestr_init(bs);

	bytestr_append(bs, tr->announce_url);
	bytestr_append(bs, "?");

	char* encoded_hash = HTTP_encode_buffer(tr->info_hash, 20);
	append_param(bs, "info_hash", encoded_hash);
	char* encoded_pid = HTTP_encode_buffer(tr->pid, 20);
	append_param(bs, "peer_id", encoded_pid);
	char* buf = alloca(21); // buffer used for all int -> str conversions
	sprintf(buf, "%d", tr->port);
	append_param(bs, "port", buf);
	sprintf(buf, "%llu", tr->uploaded);
	append_param(bs, "uploaded", buf);
	sprintf(buf, "%llu", tr->downloaded);
	append_param(bs, "downloaded", buf);
	sprintf(buf, "%llu", tr->left);
	append_param(bs, "left", buf);
	append_param(bs, "compact", (tr->compact) ? "1" : "0");
	append_param(bs, "no_peer_id", (tr->no_peer_id) ? "1" : "0");
	append_param(bs, "event", tr->event);

	return bs->ptr;
}

void send_tracker_request(struct bt_instance *instance) {
	struct tracker_req tr = {0};
	struct tracker_req *req = &tr;

	req->announce_url = instance->t_metainfo->announce;
	// req->info_hash = (uint8_t *) instance->t_metainfo->info->info_hash;
	req->info_hash = (uint8_t *) "\x99\xc8\x2b\xb7\x35\x05\xa3\xc0\xb4\x53\xf9\xfa\x0e\x88\x1d\x6e\x5a\x32\xa0\xc1"; // TODO: CHEATING !!!!!!!!!!!
	req->pid = (uint8_t *) instance->client_id;
	req->port = 6885; // TODO: mmmmmm fix
	req->uploaded = 0;
	req->downloaded = 0;
	req->left = instance->t_metainfo->info->f_info[0].length;
	req->compact = true;
	req-> no_peer_id = true;
	req->event = "started";

	char *encoded_req = get_HTTP_tracker_request(req);
	printf("req: [%s]\n", encoded_req);
	tannounce_connect("torrent.ubuntu.com", req->port, encoded_req);
}
