#include "util.h"
#include "bencode.h"

int main() {
	char *torf = "./testing/test_torrents/ubuntu-22.10-desktop-amd64.iso.torrent";
	be_node *node = load_be_node(torf);

	be_dump(node);
}
