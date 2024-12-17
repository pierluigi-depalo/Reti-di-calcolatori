/*
 * main.c
 *
 *  Created on: 4 nov 2024
 *      Author: pierd
 */

#include "socket.h"
#include "protocol.h"

int communicate(t_socket socket) {
	char buffer[DATA_MAX_LEN+1];
	char* banner = "Welcome!\nOptions:\n\tn: Numeric password (digits only)\n\ta: Alphabetical password (lowercase letters only)\n\tm: Mixed password (lowercase letters and numbers)\n\ts: Secure password (uppercase letters, lowercase letters, numbers, and symbols)\n\tu : generate unambiguous secure password (no similar-looking characters)\n\nSend your choice in the format [option] <space> [length] <enter>\n\n>: ";

	char choice; int len;
	do {
		char buffer[DATA_MAX_LEN];
		printf("%s", banner);
	    if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
	    sscanf(buffer, "%c %d", &choice, &len);
	    printf("%s\n", buffer);

		t_socket senderSocket = new_t_socket_struct();
		if(udp_send(socket, buffer, strlen(buffer), socket) != strlen(buffer)) return SEND_ERROR;

		int bytes_read = udp_recv(socket, buffer, DATA_MAX_LEN, senderSocket);
		if(bytes_read < 0) return READ_ERROR;
		buffer[bytes_read] = '\0';

		printf("%s\n", buffer);
	} while(choice != 'q');

	return 0;
}

int main() {
	t_socket client = client_socket_init_domain(TRANSPORT_PROTOCOL, "passwdgen.uniba.it", PROTOCOL_PORT);
	if(client) {
		communicate(client);
		socket_close(&client);
	}
	return 0;
}
