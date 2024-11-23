/*
 * main.c
 *
 *  Created on: 4 nov 2024
 *      Author: pierd
 */

#include "socket.h"
#include "protocol.h"
#include <time.h>

enum CHOICE {
	NUMBERS = 'n', LETTERS = 'a', NUMBERS_LETTERS = 'm', ALL_CHARS = 's'
};

int service(t_socket);
void generate_numeric(char*, int);
void generate_alpha(char*, int);
void generate_mixed(char*, int);
void generate_secure(char*, int);
void generate_password(char*, int, int, int);
void manage_choice(char*, enum CHOICE, int);
int service(t_socket);

int main() {
	t_socket server = server_socket_init(TRANSPORT_PROTOCOL, "127.0.0.1", PROTOCOL_PORT);
	socket_serve(server, service);
	socket_close(&server);
	system("PAUSE");
	return 0;
}

void generate_numeric(char* pass, int len) {
	int l = 53, r = 61;
	generate_password(pass, len, l, r);
}

void generate_alpha(char* pass, int len) {
	int l = 27, r = 51;
	generate_password(pass, len, l, r);
}

void generate_mixed(char* pass, int len) {
	int l = 0, r = 62;
	generate_password(pass, len, l, r);
}

void generate_secure(char* pass, int len) {
	int l = 0, r = 88;
	generate_password(pass, len, l, r);
}

void generate_password(char* pass, int len, int l, int r) {
	char* charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+[]{}|;:,.<>?/~`";
	for(int i=0; i<len; i++) {
		pass[i] = charset[(rand()%(r-l+1))+l];
	}
	pass[len] = '\0';
}

void manage_choice(char* pass, enum CHOICE type, int len) {
	switch(type) {
	case NUMBERS:
		generate_numeric(pass, len);
		break;
	case LETTERS:
		generate_alpha(pass, len);
		break;
	case NUMBERS_LETTERS:
		generate_mixed(pass, len);
		break;
	case ALL_CHARS:
		generate_secure(pass, len);
		break;
	}
}

int service(t_socket client) {
	char buffer[DATA_MAX_LEN+1];
	char choice, generated_pass[MAX_LENGTH+1];
	char* banner = "Welcome!\nOptions:\n\tn: Numeric password (digits only)\n\ta: Alphabetical password (lowercase letters only)\n\tm: Mixed password (lowercase letters and numbers)\n\ts: Secure password (uppercase letters, lowercase letters, numbers, and symbols)\n\nSend your choice in the format [option] <space> [length] <enter>\n\n>: ";
	printf("New connection from %s:%d\n", inet_ntoa(get_socket_settings(client).sin_addr), get_socket_settings(client).sin_port);

	strcpy(buffer, banner);
	if(send(get_socket_id(client), buffer, strlen(buffer), 0) != strlen(buffer)) return SEND_ERROR;
	do {

		int len;

		int bytes_read =  recv(get_socket_id(client), buffer, DATA_MAX_LEN, 0);
		if(bytes_read < 0) return READ_ERROR;
		buffer[bytes_read] = '\0';

		int nr = 0; int valid = 0;
		if (sscanf(buffer, "%c%n", &choice, &nr) == 1) {
		    if(buffer[nr] == ' ' && buffer[nr+1] >= '0' && buffer[nr+1] <= '9') {
				if (sscanf(buffer+nr, "%d", &len) == 1) {
					valid  = 1;
				}
		    }
		}
		if(choice != 'q') {
			if(valid == 0) {
				sprintf(buffer, "Invalid input format. Use: [option] <space> [length]\n\n%s", banner);
				if(send(get_socket_id(client), buffer, strlen(buffer), 0) != strlen(buffer)) return SEND_ERROR;
			}
			else if(choice != NUMBERS && choice != LETTERS && choice != NUMBERS_LETTERS && choice != ALL_CHARS) {
				sprintf(buffer, "Invalid choice, choose one of the option proposed\n\n%s", banner);
				if(send(get_socket_id(client), buffer, strlen(buffer), 0) != strlen(buffer)) return SEND_ERROR;
			}
			else if(len < MIN_LENGTH || len > MAX_LENGTH) {
				sprintf(buffer, "Invalid length. Password length must be between %d and %d\n\n%s", MIN_LENGTH, MAX_LENGTH, banner);
				if(send(get_socket_id(client), buffer, strlen(buffer), 0) != strlen(buffer)) return SEND_ERROR;
			}
			else {
				manage_choice(generated_pass, choice, len);
				sprintf(buffer, "Here is the password: %s\n\n%s", generated_pass, banner);
				if(send(get_socket_id(client), buffer, strlen(buffer), 0) != strlen(buffer)) return SEND_ERROR;
			}
		}
	} while (choice != 'q');
	return 0;
}
