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
	NUMBERS = 'n', LETTERS = 'a', NUMBERS_LETTERS = 'm', ALL_CHARS = 's', UNAMBIGUOUS = 'u'
};

int service(t_socket);
void generate_numeric(char*, int);
void generate_alpha(char*, int);
void generate_mixed(char*, int);
void generate_secure(char*, int);
void generate_password(char*, int, char*);
void manage_choice(char*, enum CHOICE, int);
int service(t_socket);

int main() {
	t_socket server = server_socket_init(TRANSPORT_PROTOCOL, "127.0.0.1", PROTOCOL_PORT);
	service(server);
	socket_close(&server);
	system("PAUSE");
	return 0;
}

void generate_numeric(char* pass, int len) {
	char* charset = "0123456789";
	generate_password(pass, len, charset);
}

void generate_alpha(char* pass, int len) {
	char* charset = "abcdefghijklmnopqrstuvwxyz";
	generate_password(pass, len, charset);
}

void generate_mixed(char* pass, int len) {
	char* charset = "abcdefghijklmnopqrstuvwxyz0123456789";
	generate_password(pass, len, charset);
}

void generate_secure(char* pass, int len) {
	char* charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+[]{}|;:,.<>?/~";
	generate_password(pass, len, charset);
}

void generate_unambiguous(char* pass, int len) {
	char* charset = "ACDEFGHJKLMNPQRTUVWXYacdefghjkmnpqrtuvwxy34679!@#$%^&*()_+[]{}|;:,.<>?/~";
	generate_password(pass, len, charset);
}

void generate_password(char* pass, int len, char* charset) {
	for(int i=0; i<len; i++) {
		pass[i] = charset[rand()%strlen(charset)];
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
	case UNAMBIGUOUS:
		generate_unambiguous(pass, len);
		break;
	}
}

int service(t_socket server) {
	char buffer[DATA_MAX_LEN+1];
	char choice, generated_pass[MAX_LENGTH+1];

	printf("Listening...\n");
	t_socket senderSocket = new_t_socket_struct();
	while(1) {
		int len;
		int bytes_read =  udp_recv(server, buffer, DATA_MAX_LEN, senderSocket);
		printf("New request from from %s:%d\n", inet_ntoa(get_socket_settings(senderSocket).sin_addr), get_socket_settings(senderSocket).sin_port);

		if(bytes_read < 0) return READ_ERROR;
		buffer[bytes_read] = '\0';
		int nr = 0; int valid = 0;
		if (sscanf(buffer, "%c%n", &choice, &nr) == 1) {
		    if(buffer[nr] == ' ' && buffer[nr+1] >= '0' && buffer[nr+1] <= '9') {
				if (sscanf(buffer+nr, "%d", &len) == 1) {
					valid = 1;
				}
		    }
		}

		if(valid == 0) {
			sprintf(buffer, "Invalid input format. Use: [option] <space> [length]");
			if(udp_send(server, buffer, strlen(buffer), senderSocket) != strlen(buffer)) return SEND_ERROR;
		}
		else if(choice != UNAMBIGUOUS && choice != NUMBERS && choice != LETTERS && choice != NUMBERS_LETTERS && choice != ALL_CHARS) {
			sprintf(buffer, "Invalid choice, choose one of the option proposed");
			if(udp_send(server, buffer, strlen(buffer), senderSocket) != strlen(buffer)) return SEND_ERROR;
		}
		else if(len < MIN_LENGTH || len > MAX_LENGTH) {
			sprintf(buffer, "Invalid length. Password length must be between %d and %d", MIN_LENGTH, MAX_LENGTH);
			if(udp_send(server, buffer, strlen(buffer), senderSocket) != strlen(buffer)) return SEND_ERROR;
		}
		else {
			manage_choice(generated_pass, choice, len);
			sprintf(buffer, "Here is the password: %s", generated_pass);
			if(udp_send(server, buffer, strlen(buffer), senderSocket) != strlen(buffer)) return SEND_ERROR;
		}
	}
	return 0;
}
