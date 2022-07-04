#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "network.h"
#include "fibonacci.h"
#include "error.h"

//TODO DEBUG INCLUDES, REMOVE
#include <stdio.h>


// Build socket
void build_sock(int * sock) {

	*sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (written_data->sock == -1) {
		handle_err(ERROR_SOCKET_OPEN);
	}

	//Set socket not to block to send and recv simultaneously.
	int temp = fcntl(*recv_data_srct->sock, F_SETFL, fcntl(
					 *recv_data_srct->sock, F_GETFL, 0) | O_NONBLOCK);
	if (temp == -1) handle_err(ERROR_SOCKET_NONBLOCK);

}


// Build sending packet.
void build_send(struct send_data * send_data_srct) {

	//Write packet
	send_data_srct->packet_body = 
		(char *) (send_data_srct->packet + sizeof(struct udphdr));
	send_data_srct->udp_header = (struct udphdr *)
									(send_data_srct->packet_send
									+ sizeof (struct iphdr));

	//Write header
	send_data_srct->udp_header = (struct udphdr *) send_data_srct->packet_send;
	send_data_srct->udp_header->source = htons(PORT);
	send_data_srct->udp_header->len = htons(sizeof(struct udphdr));
}

//Update sending packet. 'num' 0 sends ack instead.
void update_send(struct send_data * send_data_srct, struct host_data * host_data_srct,
				 int num_to_check) {

	char * body_content[2] = {
		"<!DOCTYPE html>\n<header>\n</header>\n<body>\n<h1>Error 404</h1>\n</body>",
		"<!DOCTYPE html>\n<header>\n</header>\n<body>\n<h1>Error 403</h1>\n</body>"
	};

	//Fill body.
	send_data_srct->udp_header->dest = host_data_srct->addr->sin_port;
	memset(send_data_srct->packet, 0, sizeof(send_data_srct->packet));
	strncpy(send_data_srct->packet_body, body_content[ack], strlen(body_content[ack]));

	//Set ack or number to check. 0 = ack.
	send_data_srct->udp_header->check = num_to_check;

}


// Build recv. Also used for resetting the recv packet.
void build_recv(struct recv_data * recv_data_srct) {

	memset(recv_data_srct->packet_recv, 0, DATAGRAM_SIZE);
	recv_data_srct->packet_recv_body = (char *) (recv_data_srct->packet_recv
									 + sizeof(struct iphdr)
									 + sizeof(struct udphdr));
	recv_data_srct->udp_header = (struct udphdr *)
									(recv_data_srct->packet_recv
									+ sizeof (struct iphdr));
}


// Send number for fibonacci sequence.
int try_send(struct send_data * send_data_srct, struct host_data * host_data_srct, 
			 int num_to_check) {

	send_data_srct->udp_header->dest = htons(host_data_srct->addr->dest);
	send_data_srct->udp_header->check = num_to_check;

	ssize_t sent = sendto(send_data_srct->sock, 
						  send_data_srct->packet,
						  (sizeof(struct udphdr)+strlen(send_data_srct->packet_body)+1),
						  0,
						  (struct sockaddr *) &host_data_srct->addr,
						  sizeof(struct sockaddr_in));
	return sent;
}


// Recv data via UDP.
int try_recv(struct recv_data * recv_data_srct, struct sockaddr_in * addr) {

	socklen_t len;

	ssize_t recved = recvfrom(*recv_data_srct->sock,
			recv_data_srct->packet_recv,
			(sizeof(struct iphdr)+sizeof(struct udphdr)
			+ strlen(recv_data_srct->packet_recv_body) + 1),
			0,
			addr,
			&len);

	return recved;

}


// Update ack time
void set_ack_time(struct host_data * host) {

	if (gettimeofday(&host->ack_time) != 0) handle_err(ERROR_TIME_GETTIME);

}


/*
 * The program assumes UDP packets wont be dropped n number of times in a row,
 * in the current use case, 3 times in a row. If they are, the program assumes
 * host has lost connection. Connection is immediately reistablished however
 * once another ping arrives.
 */

// Check outdated ack time, 0 = ok, 1 = outdated
int check_outdated_ack_time(struct host_data * host, struct timeval * time) {

	struct timeval cur_time;
	if (gettimeofday(&cur_time, NULL) != 0) handle_err(ERROR_TIME_GETTIME);

	//If last ping has passed HOST_DISCONNECT_TIMER boundry
	if(host->ack_time.tv_sec + HOST_DISCONNECT_TIMER < cur_time.tv_sec) {
		return 1;
	
	//Else ping still in bounds
	} else {
		return 0;
	}

}


// Get input from api. 0 = None, 1 = Some
int api_get_input() {
	
	//Dummy
	return 0;
}

// Send output back tp api
int api_send_output(uint16_t in_fibonacci) {

	//Dummy
	return 0	
}
