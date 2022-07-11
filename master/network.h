#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define PORT 80
#define PORT_API 25000
#define DATAGRAM_SIZE 256//1024
#define DATAGRAM_SIZE_RECV 276//1044 //IP header takes extra 20 bytes.
#define MAX_HOST 128
#define HOST_DISCONNECT_TIMER 15 //Seconds.
#define API_GET_SIZE 8

/*	Note about the above API_GET_SIZE. Currently as per recommendation
 *  it is set to 1500 - the max packet size. However, since this is a
 *	regular socket, the buffer only stores the content of the body.
 *	As per API validation, the max length should be 5-6 characters.
 *	The previous value was 8, a nice round number that fits max inp size.
 */

#define HOST_STATE_DISCONNECTED 0
#define HOST_STATE_PINGING 1
#define HOST_STATE_WORK_SENT 2

#define API_CONN_SUCCESS 1
#define API_CONN_FAIL 0

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

struct host_data {

	struct sockaddr_in addr;
	struct timeval ack_time;	
	uint8_t state; //0 - disconnected, 1 - pinging, 2 - work sent
};

struct api_data {

	struct sockaddr_in addr_listen;
	struct sockaddr_in addr_api;
	char ret_buf[API_GET_SIZE]; //Also used for sending back to API.

};

struct recv_data {

	char packet_recv[DATAGRAM_SIZE_RECV];
	char * packet_recv_body; //stores body
	struct iphdr * ip_header;
	struct udphdr * udp_header; //struct version, points to same thing.
	struct sockaddr_in addr;

};


//All the networking functions.
void build_sock(int * sock);

void build_send(struct send_data * send_data_srct);
void update_send(struct send_data * send_data_srct, struct host_data * host_data_srct, uint16_t num_to_check);

void build_api(struct api_data * api_data_srct, int * sock_listen);

void build_recv(struct recv_data * recv_data_srct);

int try_send(struct send_data * send_data_srct, struct host_data * host_data_srct, int * sock, uint16_t num_to_check);
int try_recv(struct recv_data * recv_data_srct, int * sock);

void set_ack_time(struct host_data * host);
int check_outdated_ack_time(struct host_data * host, struct timeval * time);

int api_accept_conn(struct api_data * api_data_srct, int * sock_listen,
					 int * sock_api);
uint16_t api_get_input(int * sock_api, struct api_data * api_data_srct);
int api_send_output(int * sock_api, struct api_data * api_data_srct, 
					uint16_t in_fibonacci);

#endif
