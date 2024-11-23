/*
 * main.c
 *
 *  Created on: 4 nov 2024
 *      Author: pierd
 */

#include "socket.h"
#include "protocol.h"

int communicate(t_socket server) {
	char buffer[DATA_MAX_LEN+1];
	printf("Connected to %s\n", inet_ntoa(get_socket_settings(server).sin_addr));

	char choice; int len;
	do {
		int bytes_read = recv(get_socket_id(server), buffer, DATA_MAX_LEN, 0);
		if(bytes_read < 0) return READ_ERROR;
		buffer[bytes_read] = '\0';
		printf("%s", buffer);

		char buffer[DATA_MAX_LEN];
	    if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
	    sscanf(buffer, "%c %d", &choice, &len);

	    if(send(get_socket_id(server), buffer, strlen(buffer), 0) != strlen(buffer)) return SEND_ERROR;
	} while(choice != 'q');

	return 0;
}

int main() {
	char* server_ip = "127.0.0.1";
	t_socket client = client_socket_init(TRANSPORT_PROTOCOL, server_ip, PROTOCOL_PORT);
	communicate(client);
	socket_close(&client);
	return 0;
}
