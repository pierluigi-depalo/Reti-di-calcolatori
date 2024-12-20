/*
 * main.c
 *
 *  Created on: 4 nov 2024
 *      Author: pierd
 */

#include "socket.h"
#include "protocol.h"

void show_help_menu() {
	printf("\nPassword Generator Help Menu\n"
	"Commands:\n"
	"\th        : show this help menu\n"
	"\tn LENGTH : generate numeric password (digits only)\n"
	"\ta LENGTH : generate alphabetic password (lowercase letters)\n"
	"\tm LENGTH : generate mixed password (lowercase letters and numbers)\n"
	"\ts LENGTH : generate secure password (uppercase, lowercase, numbers, symbols)\n"
	"\tu LENGTH : generate unambiguous secure password (no similar-looking characters)\n"
	"\tq        : quit application\n"
	"\nLENGTH must be between %d and %d characters\n"
	"\nAmbiguous characters excluded in 'u' option:\n"
	"\t0 O o (zero and letters O)\n"
	"\t1 l I i (one and letters l, I)\n"
	"\t2 Z z (two and letter Z)\n"
	"\t5 S s (five and letter S)\n"
	"\t8 B (eight and letter B)\n", MIN_LENGTH, MAX_LENGTH);
}

int communicate(t_socket socket) {
	char buffer[DATA_MAX_LEN+1];
	char choice; int len;
	t_socket senderSocket = new_t_socket_struct();
	show_help_menu();
	do {
		printf(">: ");
	    if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
	    sscanf(buffer, "%c %d", &choice, &len);

	    if(choice != 'q') {
	    	if(choice != 'h') {
				if(udp_send(socket, buffer, strlen(buffer), socket) != strlen(buffer)) return SEND_ERROR;

				int bytes_read = udp_recv(socket, buffer, DATA_MAX_LEN, senderSocket);
				if(bytes_read < 0) return READ_ERROR;
				buffer[bytes_read] = '\0';

				printf("%s\n", buffer);
	    	}
	    	else {
	    		show_help_menu();
	    	}
	    }
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
