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
}

//Update sending packet. 'num' = 0 sends ack instead.
void update_send(struct send_data * send_data_srct, 
				 struct master_data * master_data_srct, uint16_t num) {
	
	int content_index;
	//0 = ack, 1 = return of calc
	if (num > 0) {content_index = 1;} else {content_index = 0;}
	//The following mimics the body of a gopher protocol request.
	char * body_content[2] = {
		"/welcome.txt",
		"/about.txt"
	};

	//Fill body.
	memset(send_data_srct->packet_send, 0, sizeof(send_data_srct->packet_send));
	strncpy(send_data_srct->packet_send_body, body_content[content_index],
			strlen(body_content[content_index]));

	//Reassign ports. While this may seem unnecessary, it somehow isn't.
	send_data_srct->udp_header->source = htons(master_data_srct->port);
	send_data_srct->udp_header->dest = htons(PORT);

	printf("---SOURCE PORT: %u\n", ntohs(send_data_srct->udp_header->source));
	printf("---  DEST PORT: %u\n", ntohs(send_data_srct->udp_header->dest));

	//Set header len to 8;
	send_data_srct->udp_header->len = htons(8 + strlen(send_data_srct->packet_send_body));

	//Set ack or number to check. 0 = ack.
	send_data_srct->udp_header->check = num;

}


// Build master.
void build_master(struct master_data * master_data_srct) {

	master_data_srct->addr.sin_port = htons(master_data_srct->port);
	master_data_srct->addr.sin_addr.s_addr = inet_addr(master_data_srct->ip);
	master_data_srct->addr.sin_family = AF_INET;
}


// Build recv. Also used for resetting the recv packet.
void build_recv(struct recv_data * recv_data_srct,
				struct master_data * master_data_srct) {

	memset(recv_data_srct->packet_recv, 0, DATAGRAM_SIZE);
	memset(&recv_data_srct->addr, 0, sizeof(struct sockaddr_in));
	recv_data_srct->packet_recv_body = (char *) (recv_data_srct->packet_recv
									 + sizeof(struct iphdr)
									 + sizeof(struct udphdr));
	recv_data_srct->udp_header = (struct udphdr *)
									(recv_data_srct->packet_recv
									+ sizeof (struct iphdr));

	//Specify address of sender. Otherwise all UDP traffic is caught which
	//breaks the program.
	recv_data_srct->addr.sin_port = htons(PORT);
	recv_data_srct->addr.sin_addr.s_addr = inet_addr(master_data_srct->ip);
}


// Send number for fibonacci sequence.
int try_send(struct send_data * send_data_srct,
			 struct master_data * master_data_srct, int * sock) {


	ssize_t sent = sendto(*sock, 
						  send_data_srct->packet_send,
						  (sizeof(struct udphdr)+strlen(send_data_srct->packet_send_body)+1),
						  0,
						  (const struct sockaddr *)&master_data_srct->addr,
						  sizeof(struct sockaddr_in));
	

	printf("SEND %ld BYTES\n", sent);

	return sent;
}


// Recv data via UDP.
int try_recv(struct recv_data * recv_data_srct, int * sock) {

	struct sockaddr_in recv_data_all;

	ssize_t recved = recvfrom(*sock,
			recv_data_srct->packet_recv,
			DATAGRAM_SIZE,
			0,
			NULL,
			NULL);

	//Drop all packets not from master.
	if (recv_data_all.sin_addr.s_addr != recv_data_srct->addr.sin_addr.s_addr) {
		return -1;
	}

	//If body matches the body that master sends
	if (strcmp(recv_data_srct->packet_recv, ":)") == 0
			|| strcmp(recv_data_srct->packet_recv, ":^)") == 0) {
		return recved;
	//If body doesn't match.
	} else {
		return -1;
	}
}
