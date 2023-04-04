#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "bt_client.h"

#define CLIENT_ID "ST"
#define CLIENT_VER "0000"

// generates null terminated id
char *generate_id() {
	char *id = malloc(21);

	memcpy(id, "-", 1);
	memcpy(id + 1, CLIENT_ID, 2);
	memcpy(id + 3, CLIENT_VER, 4);
	memcpy(id + 7, "-", 1);
	char uid[12] = {0}; // TODO: change to random id later
	memset(uid, '0', 12);
	memcpy(id + 8, uid, 12);
	memcpy(id + 20, "\0", 1);

	return id;
}

void connect_tracker(struct bt_instance *instance) {
	char *sha = HTTP_encode_buffer((uint8_t *) instance->t_metainfo->info->sha_hashes[0].hash, 20);
	char *pid = HTTP_encode_buffer((uint8_t *) instance->client_id, 20);


}
