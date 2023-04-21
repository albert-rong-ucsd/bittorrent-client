#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

struct bt_fileinfo {
	char *name;                // name of file
	unsigned long long length; // length of file in bytes
	char *path;                // path to file
};

struct sha_hash {
	char hash[20];
};

struct bt_info {
	struct bt_fileinfo *f_info;  // array of ptrs to info on each file in torrent
	char info_hash[20];          // 20-byte sha-1 hash of the bencoded info
	char *name;                  // name of torrent (filename if single file)
	int num_files;               // number of files in torrent
	int piece_length;            // number of bytes in each piece
	int num_pieces;              // number of pieces in file (derived)
	struct sha_hash *sha_hashes; // points to concatenation of 20-byte SHA1 hashes of each piece
};

struct bt_metainfo {
	struct bt_info *info;       // info for torrent file
	char *announce;             // url of tracker
								// XXX: announce list not included at the moment
	time_t *created;            // creation time of torrent
	char *comment;              // comments by author
	char *creator;              // name + version of program that created .torrent
	char *encoding;             // str encode format of info dictionary
};

struct bt_instance { // holds state of client
	struct bt_metainfo *t_metainfo; // only works on 1 torrent at the moment
	char *client_id;                // peer id used to identify the client

};

struct tracker_req {
	char *announce_url;
	uint8_t *info_hash;
	uint8_t *pid;
	uint16_t port;
	unsigned long long uploaded;
	unsigned long long downloaded;
	unsigned long long left;
	bool compact;
	bool no_peer_id;
	char *event;
};

void populate_torrent_metainfo(struct bt_metainfo *metainfo, char *filepath);
void print_torrent_metainfo(struct bt_metainfo *metainfo);
char *HTTP_encode_buffer(uint8_t *byte_str, size_t len);
void send_tracker_request(struct bt_instance *instance);

#endif
