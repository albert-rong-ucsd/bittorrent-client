#ifndef UTIL_H
#define UTIL_H

#include <time.h>

struct bt_fileinfo {
	char *name;                // name of file
	unsigned long long length; // length of file in bytes
	char *path;                // path to file
};

struct bt_info {
	struct bt_fileinfo **f_info; // array of ptrs to info on each file in torrent
	int num_files;               // number of files in torrent
	int piece_length;            // number of bytes in each piece
	int num_pieces;              // number of pieces in file (derived)
	char **piece_hashes;         // points to concatenation of 20-byte SHA1 hashes of each piece
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

#endif
