#ifndef BT_CLIENT_H
#define BT_CLIENT_H

#include <stdbool.h>
#include <stdint.h>

#include "util.h"

struct bt_peer { // holds information about a torrent peer
	bool choked;
	bool interested;
	uint32_t ip;
	uint16_t port;
};

char *generate_id(); // TODO : private later

#endif
