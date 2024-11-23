#ifndef SOCK_WRAP
#define SOCK_WRAP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <winsock.h>

#else

#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#endif



enum SOCKET_TYPE { TCP = SOCK_STREAM, UDP = SOCK_DGRAM};

typedef struct s_socket* t_socket;

enum SOCKET_ERRORS {
    BIND_ERROR = -1,
    ALLOCATION_ERROR = -2,
    LISTEN_ERROR = -3,
    ACCEPT_ERROR = -4,
    HANDLING_ERROR = -5,
	SEND_ERROR = -6,
	READ_ERROR = -7
};

enum SOCKET_MODE {
    SERVER,
    CLIENT
};

t_socket get_new_socket(enum SOCKET_TYPE type);
int socket_dll_load();

t_socket server_socket_init(enum SOCKET_TYPE type, char* address, int port);
t_socket client_socket_init(enum SOCKET_TYPE type, char* address, int port);
t_socket socket_init(enum SOCKET_TYPE type, char* address, int port, enum SOCKET_MODE mode);

int socket_listen(t_socket socket);
int socket_serve(t_socket socket, int (*funct)(t_socket));
void socket_close(t_socket* socket);
void socket_clear();
struct sockaddr_in get_socket_settings(t_socket sock);
int get_socket_id(t_socket sock);

#endif
