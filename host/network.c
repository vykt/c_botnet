#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

#include "network.h"
#include "fibonacci.h"
#include "error.h"

//TODO DEBUG INCLUDES, REMOVE
#include <stdio.h>


// Build socket
void build_sock(int * sock) {

	*sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (*sock == -1) {
		handle_err(ERROR_SOCKET_OPEN);
	}

	//Set socket not to block to send and recv simultaneously.
	int temp = fcntl(*sock, F_SETFL, fcntl(
					 *sock, F_GETFL, 0) | O_NONBLOCK);
	if (temp == -1) handle_err(ERROR_SOCKET_NONBLOCK);

}


// Build sending packet.
void build_send(struct send_data * send_data_srct, struct master_data * master_data_srct) {

	//Write packet
	send_data_srct->packet_send_body = 
		(char *) (send_data_srct->packet_send + sizeof(struct udphdr));
	send_data_srct->udp_header = (struct udphdr *)
									(send_data_srct->packet_send
									+ sizeof (struct iphdr));

	//Write header
	send_data_srct->udp_header = (struct udphdr *) send_data_srct->packet_send;
	send_data_srct->udp_header->source = htons(master_data_srct->port);
	send_data_srct->udp_header->dest = htons(PORT);
	send_data_srct->udp_header->len = htons(sizeof(struct udphdr));
}

//Update sending packet. 'num' 0 sends ack instead.
void update_send(struct send_data * send_data_srct, uint16_t num) {
	
	int content_index;
	//0 = ack, 1 = return of calc
	if (num > 0) {content_index = 1;} else {content_index = 0;}
	char * body_content[2] = {
		"HTTP GET index.html",
		"HTTP GET www/index.html"
	};

	//Fill body.
	memset(send_data_srct->packet_send, 0, sizeof(send_data_srct->packet_send));
	strncpy(send_data_srct->packet_send_body, body_content[content_index],
			strlen(body_content[content_index]));

	//Set ack or number to check. 0 = ack.
	send_data_srct->udp_header->check = num;

}


// Build recv. Also used for resetting the recv packet.
void build_recv(struct recv_data * recv_data_srct) {

	memset(recv_data_srct->packet_recv, 0, DATAGRAM_SIZE);
	memset(&recv_data_srct->addr, 0, sizeof(struct sockaddr_in));
	recv_data_srct->packet_recv_body = (char *) (recv_data_srct->packet_recv
									 + sizeof(struct iphdr)
									 + sizeof(struct udphdr));
	recv_data_srct->udp_header = (struct udphdr *)
									(recv_data_srct->packet_recv
									+ sizeof (struct iphdr));
}


// Send number for fibonacci sequence.
int try_send(struct send_data * send_data_srct, int * sock) {

	ssize_t sent = sendto(*sock, 
						  send_data_srct->packet_send,
						  (sizeof(struct udphdr)+strlen(send_data_srct->packet_send_body)+1),
						  0,
						  NULL,
						  sizeof(struct sockaddr_in));
	return sent;
}


// Recv data via UDP.
int try_recv(struct recv_data * recv_data_srct, int * sock) {

	socklen_t len;

	ssize_t recved = recvfrom(*sock,
			recv_data_srct->packet_recv,
			(sizeof(struct iphdr)+sizeof(struct udphdr)
			+ strlen(recv_data_srct->packet_recv_body) + 1),
			0,
			&recv_data_srct->addr,
			&len);

	return recved;

}
