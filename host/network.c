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

	//Packet layout
	send_data_srct->udp_header = (struct udphdr *) send_data_srct->packet_send;
	send_data_srct->packet_send_body = (char *) 
									   (send_data_srct->udp_header + 1);

	//Address
	send_data_srct->udp_header->source = htons(master_data_srct->port);
	send_data_srct->udp_header->dest = htons(PORT);
	
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
	memset(send_data_srct->packet_send, 0, DATAGRAM_SIZE);
	strncpy(send_data_srct->packet_send_body, body_content[content_index],
			strlen(body_content[content_index]));

	//Set ports
	send_data_srct->udp_header->source = htons(master_data_srct->port);
	send_data_srct->udp_header->dest = htons(PORT);

	//Set header len
	send_data_srct->udp_header->len = 
		htons(8 + strlen(send_data_srct->packet_send_body));

	//Set ack or number to check. 0 = ack.
	send_data_srct->udp_header->check = htons(num);

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
	
	recv_data_srct->ip_header = (struct iphdr *) recv_data_srct->packet_recv;
	recv_data_srct->udp_header = (struct udphdr *) 
								 (recv_data_srct->ip_header + 1);
	recv_data_srct->packet_recv_body = (char *) 
									   (recv_data_srct->udp_header + 1);
}


// Send number for fibonacci sequence.
ssize_t try_send(struct send_data * send_data_srct,
			 struct master_data * master_data_srct, int * sock) {
	
	ssize_t sent = sendto(*sock, 
						  send_data_srct->packet_send,
						  (sizeof(struct udphdr)
						   + strlen(send_data_srct->packet_send_body) + 1),
						  0,
						  (const struct sockaddr *)&master_data_srct->addr,
						  sizeof(struct sockaddr_in));

	return sent;
}


// Recv data via UDP.
int try_recv(struct recv_data * recv_data_srct,
			 struct master_data * master_data_srct, int * sock) {

	socklen_t len = sizeof(recv_data_srct->addr);

	ssize_t recved = recvfrom(*sock,
			recv_data_srct->packet_recv,
			DATAGRAM_SIZE,
			0,
			(struct sockaddr *)&recv_data_srct->addr,
			&len);
	
	char * addr_recv = inet_ntoa(recv_data_srct->addr.sin_addr);
	char * addr_master = inet_ntoa(master_data_srct->addr.sin_addr);


	//Drop all packets not from master.
	if (strcmp(addr_recv, addr_master) != 0) {
		return -1;
	}

	//Also check body of message, just in case.
	if (strcmp(recv_data_srct->packet_recv_body, 
			   "This gopher hole is under construction.") != 0) {
		return -1;
	}

	//Drop all packets from master addressed to other hosts
	
	/*
	 *	This here is only useful during testing, and to prove that docker
	 *	is broken. By checking destination ports, 2 hosts on the same
	 *	machine can act as completely separate entities.
	 */

	if ((uint16_t) master_data_srct->port 
			!= ntohs(recv_data_srct->udp_header->dest)) {
		return -1;
	}

	return recved;
}
