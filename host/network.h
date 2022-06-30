#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <netinet/in.h>


struct init_data {

	u_short port;
	char ip[40];

};

struct conn_data {

	int sock;
	struct sockaddr_in6 addr6;

};


int build_conn(struct conn_data * master_conn, struct init_data * master_init);
int try_connect();


#endif
