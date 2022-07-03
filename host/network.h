#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define PORT 80

#define DATAGRAM_SIZE 1024


//Conn struct, written by build_conn()
struct conn_data {

	int sock;
	char packet[DATAGRAM_SIZE];
	char * packet_body;
	struct sockaddr_in src_addr;
	struct sockaddr_in dst_addr;
	struct udphdr * udp_header;
};

struct pass_data {

	char ip[16];
	int port;
};

struct recv_data {

	int * sock;
	char packet_recv[DATAGRAM_SIZE];
	char * packet_recv_body;
	struct udphdr * packet_recv_check; //stores number received
	struct sockaddr_in addr;

};


int build_conn(struct conn_data * written_data, struct pass_data * passed_data);
int build_recv(struct recv_data * written_recv_data, struct conn_data * written_data);

int try_send(struct conn_data * conn_data_srct);
int try_recv(struct recv_data * recv_data_srct);

#endif
