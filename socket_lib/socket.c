#include "socket.h"

// If no params are specified, the library defaults to these DATA_MAX_LEN and QUEUE_LEN

#ifndef DATA_MAX_LEN
#define DATA_MAX_LEN 512
#endif

#ifndef QUEUE_LEN
#define QUEUE_LEN 5
#endif

typedef struct s_socket {
    int id;
    enum SOCKET_TYPE type;
    struct sockaddr_in settings;
} s_socket;

/* PRIVATE PART OF LIBRARY */

int change_socket_counter(int diff) {
    static int counter = 0;
    counter += diff;
    return counter;
}

int get_socket_counter() {
    return change_socket_counter(0);
}

int increase_socket_counter() {
    return change_socket_counter(1);
}

int decrease_socket_counter() {
    return change_socket_counter(-1);
}

void set_socket_settings(t_socket sock, struct sockaddr_in ns) {
    sock->settings.sin_family = ns.sin_family;
    sock->settings.sin_addr.s_addr = ns.sin_addr.s_addr;
    sock->settings.sin_port = ns.sin_port;
}

void set_socket_id(t_socket sock, int id) {
    sock->id = id;
}

/* PUBLIC PART OF LIBRARY */

t_socket new_t_socket_struct() {
    t_socket ret = (t_socket)calloc(1, sizeof(struct s_socket));
    return ret;
}

t_socket new_t_socket() {
    t_socket ret = new_t_socket_struct();
    increase_socket_counter();
    return ret;
}

t_socket get_new_socket(enum SOCKET_TYPE type) {
    t_socket ret = NULL;
    if(socket_dll_load() == 0) {
        int new_socket;
        new_socket = socket(PF_INET, type, (type == TCP ? IPPROTO_TCP : IPPROTO_UDP));
        if (new_socket >= 0) {
            ret = new_t_socket();
            set_socket_id(ret, new_socket);
        }
    }
    return ret;
}

int socket_dll_load() {
    int result = 0;

    #ifdef _WIN32
    if(get_socket_counter() == 0) {
        WSADATA wsa_data;
        result = WSAStartup(MAKEWORD(2,2), &wsa_data);
        if (result) {
            //printf("Error at WSAStartup()\n");
            //printf("A usable WinSock DLL cannot be found");
        }
    }
    #endif
    
    return result;
}

t_socket server_socket_init(enum SOCKET_TYPE type, char* address, int port) {
    return socket_init(type, address, port, SERVER);
}

t_socket client_socket_init_domain(enum SOCKET_TYPE type, char* domain, int port) {
	socket_dll_load();
	struct hostent *address= gethostbyname(domain);
	t_socket ret = NULL;
	if(address) {
		char* server_ip = inet_ntoa(*(struct in_addr *)address->h_addr);
		ret = client_socket_init(type, server_ip, port);
	}
	else {
		printf("Error resolving hostname\n");
        fprintf(stderr, "gethostbyname failed for %s\n", domain);
	}
    return ret;
}

t_socket client_socket_init(enum SOCKET_TYPE type, char* address, int port) {
    return socket_init(type, address, port, CLIENT);
}

t_socket socket_init(enum SOCKET_TYPE type, char* address, int port, enum SOCKET_MODE mode) {
    t_socket ret = get_new_socket(type);
    if(ret) {
        struct sockaddr_in sad;
        memset(&sad, 0, sizeof(sad));

        sad.sin_family = AF_INET;
        sad.sin_addr.s_addr = inet_addr(address);
        sad.sin_port = htons(port);
        
        int check;

        if(mode == SERVER) {
            check = bind(get_socket_id(ret), (struct sockaddr*) &sad, sizeof(sad));
        }
        else {
            check = connect(get_socket_id(ret), (struct sockaddr*) &sad, sizeof(sad));
        }
        
        if (check < 0) {
        	socket_close(&ret);
        }
        else {
            set_socket_settings(ret, sad);
        }
    }
    return ret;
}

int socket_listen(t_socket socket) {
    int ret = 0;
    if (listen(get_socket_id(socket), QUEUE_LEN) < 0) {
        ret = LISTEN_ERROR;
    }
    return ret;
}

int socket_serve(t_socket socket, int (*funct)(t_socket)) {
    int tmp = socket_listen(socket);
    if(tmp < 0) return tmp;
    struct sockaddr_in cad; // structure for the client address
    int client_socket; // socket descriptor for the client
    int client_len; // the size of the client address
    printf("SERVING...\n");
    while(1) {
        client_len = sizeof(cad);
    	client_socket = accept(get_socket_id(socket), (struct sockaddr*)&cad, &client_len);
        if(client_socket < 0) {
            return ACCEPT_ERROR;
        }

        t_socket client = new_t_socket();
    	set_socket_id(client, client_socket);
    	set_socket_settings(client, cad);

        tmp = funct(client);
        socket_close(&client);
        if(tmp < 0) {
            printf("An error occured during communication, connection lost :(\n");
            continue;
        }
    }
    return 0;
}

int udp_recv(t_socket sock, char* buffer, int maxlen, t_socket senderSocket) {
    int cliAddrLen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientInfo;
	int bytes_read =  recvfrom(get_socket_id(sock), buffer, maxlen, 0, (struct sockaddr*)&clientInfo, &cliAddrLen);
	set_socket_settings(senderSocket, clientInfo);
	return bytes_read;
}

int udp_send(t_socket sock, char* buffer, int len, t_socket toSocket) {
	struct sockaddr_in clientInfo = get_socket_settings(toSocket);
	int bytes_sent = sendto(get_socket_id(sock), buffer, len, 0, (struct sockaddr *)&clientInfo, sizeof(clientInfo));
	return bytes_sent;
}

void socket_close(t_socket* socket) {
    closesocket(get_socket_id(*socket));
    free(*socket);
    decrease_socket_counter();
    *socket = NULL;
    socket_clear();
}

void socket_clear() {
    #if defined WIN32
    if(get_socket_counter() == 0) {
        WSACleanup();
    }
    #endif
}

struct sockaddr_in get_socket_settings(t_socket sock) {
    return sock->settings;
}

int get_socket_id(t_socket sock) {
    return sock->id;
}
