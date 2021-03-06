#include "test.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	const uint32_t net = 0x0a0a0a00;
	char *password;

	setup_test("tcpr-test", "test-filter-recovery");
	password = get_password(htonl(net | 2), 8888, htonl(net | 3), 9999);

	fprintf(stderr, "Application: \"a\"\n");
	send_segment(internal_log, net | 4, net | 2, 9999, 8888, TH_ACK,
			0xbeefbead, 0xbabedeed, 0, NULL, 2, "a", password);

	fprintf(stderr, "     Filter: update (failure)\n");
	recv_update(net | 2, net | 4, 8888, 9999, 0, 0, 0, 0, 0, 0);

	fprintf(stderr, "Application: update\n");
	send_update(net | 2, net | 4, 8888, 9999, 0xbeefbead, 0xbabedeed - 4,
			peer_mss, peer_ws, 0, TCPR_HAVE_ACK);

	fprintf(stderr, "     Filter: ACK\n");
	recv_segment(external_log, net | 3, net | 2, 9999, 8888, TH_ACK,
			0xbeefbead + 2, 0xbabedeed - 4, 0, NULL, 0, NULL, password);

	fprintf(stderr, "Application: \"a\" (retransmit)\n");
	send_segment(internal_log, net | 4, net | 2, 9999, 8888, TH_ACK,
			0xbeefbead, 0xbabedeed, 0, NULL, 2, "a", password);
	recv_segment(external_log, net | 3, net | 2, 9999, 8888, TH_ACK,
			0xbeefbead, 0xbabedeed - 4, 0, NULL, 2, "a", password);

	cleanup_connection(net | 2, net | 4, 8888, 9999, 0xbeefbead,
				0xbabedeed - 4, 0);

	cleanup_test();
	return EXIT_SUCCESS;
}
