#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errono.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "network.h"
#include "fibonacci.h"
#include "error.h"

//TODO DEBUG INCLUDES, REMOVE
#include <stdio.h>


// Build socket & address.
int build_conn() {

	//This generates the IP header automatically, leaves program to define UDP header.
	master_conn->sock = socket(AF_INET6, SOCK_RAW, IPPROTO_UDP);




// OLD CODE
/*
	// Building socket for connection to master.
	master_conn->sock = socket(AF_INET6, SOCK_STREAM, 0);

	// Building address for connecton to master.
	master_conn->addr6.sin6_family = AF_INET6;
	master_conn->addr6.sin6_port = htons(master_init->port);
	inet_pton(AF_INET6, master_init->ip, &master_conn->addr6.sin6_addr);

	close(master_conn->sock);

*/
	return 0;
}


// Try to connect to master.
int try_connect() {

	return 0;
}
