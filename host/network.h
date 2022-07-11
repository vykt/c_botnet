#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define PORT 80
#define DATAGRAM_SIZE 256//1024
#define MAX_HOST 128
#define HOST_DISCONNECT_TIMER 15 //seconds
#define HOST_NAME_MAX 64

#define HOST_STATE_DISCONNECTED 0
#define HOST_STATE_PINGING 1
#define HOST_STATE_WORK_SENT 2

/*
 *	'send_data' & 'recv_data' are identical structs. Throughout development
 *	this has not been the case the majority of the time. In the future,
 *	they can be combined into a single struct.
 */


//Conn struct, written by build_conn().
struct send_data {

	char packet_send[DATAGRAM_SIZE];
	char * packet_send_body;
	struct udphdr * udp_header;
};

struct master_data {

	struct sockaddr_in addr;
	char ip[16];
	int port; //source port
};

struct recv_data {

	char packet_recv[DATAGRAM_SIZE];
	char * packet_recv_body; //stores body
	struct iphdr * ip_header;
	struct udphdr * udp_header; //struct version, points to same thing.
	struct sockaddr_in addr;

};


void build_sock(int * sock);
void build_send(struct send_data * send_data_srct, struct master_data * master_data_srct);
void update_send(struct send_data * send_data_srct, struct master_data * master_data_srct, uint16_t num);
void build_master(struct master_data * master_data_srct);
void build_recv(struct recv_data * recv_data_srct, struct master_data * master_data_srct);

ssize_t try_send(struct send_data * send_data_srct,
			 struct master_data * master_data_srct, int * sock);
int try_recv(struct recv_data * recv_data_srct, 
			 struct master_data * master_data_srct, int * sock);

#endif
