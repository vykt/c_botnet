#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
 *	Raw sockets make me want to kms
 *
 */

//Conn struct, written by build_conn()
struct conn_data {

	int sock;
	struct sockaddr_in addr;

}





int build_conn();
int try_connect();


#endif
