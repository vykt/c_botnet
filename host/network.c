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


// Build socket & address.
int build_conn(struct conn_data * written_data, struct pass_data * passed_data) {

	char * body_content = "GET index.html";

	//Start filling in connection data
	
	//Create socket, treat fail.
	printf("opening socket...\n");
	written_data->sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (written_data->sock == -1) {
		handle_err(ERROR_SOCKET_OPEN);
	}

	//Create address
	printf("creating destination address...\n");
	written_data->dst_addr.sin_family = AF_INET;
	written_data->dst_addr.sin_port = htons(PORT);
	written_data->dst_addr.sin_addr.s_addr = inet_addr(passed_data->ip);

	//Write packet
	printf("creating packet...\n");
	memset(written_data->packet, 0, sizeof(written_data->packet));
	written_data->packet_body = (char *) (written_data->packet + sizeof(struct udphdr));
	strncpy(written_data->packet_body, body_content, strlen(body_content));

	//Write header
	printf("creating header...\n");
	written_data->udp_header = (struct udphdr *) written_data->packet;
	written_data->udp_header->source = htons(passed_data->port);
	written_data->udp_header->dest = htons(PORT);
	written_data->udp_header->len = htons(sizeof(struct udphdr));

	return 0;
}


// Build recv
int build_recv(struct recv_data * written_recv_data, struct conn_data * written_data) {

	written_recv_data->sock = &written_data->sock;
	memset(written_recv_data->packet_recv, 0, DATAGRAM_SIZE);
	written_recv_data->packet_recv_body = (char *) (written_recv_data->packet_recv
									 + sizeof(struct iphdr)
									 + sizeof(struct udphdr));
	written_recv_data->packet_recv_check = (struct udphdr *)
										   (written_recv_data->packet_recv
										   + sizeof (struct iphdr));
	written_recv_data->addr = written_data->dst_addr;


	//Set socket not to block to send and recv simultaneously.
	int temp = fcntl(*written_recv_data->sock, F_SETFL, fcntl(
					 *written_recv_data->sock, F_GETFL, 0) | O_NONBLOCK);
	if (temp == -1) handle_err(ERROR_SOCKET_NONBLOCK);

	return 0;
}

// Send data via UDP.
int try_send(struct conn_data * conn_data_srct) {

	ssize_t sent = sendto(conn_data_srct->sock, 
						  conn_data_srct->packet,
						  (sizeof(struct udphdr)+strlen(conn_data_srct->packet_body)+1),
						  0,
						  (struct sockaddr *) &conn_data_srct->dst_addr,
						  sizeof(struct sockaddr_in));

	return sent;
}

// Recv data via UDP.
int try_recv(struct recv_data * recv_data_srct) {

	socklen_t len;

	ssize_t recved = recvfrom(*recv_data_srct->sock,
			recv_data_srct->packet_recv,
			(sizeof(struct iphdr)+sizeof(struct udphdr)
			+ strlen(recv_data_srct->packet_recv_body) + 1),
			0,
			NULL, //Might need to be changed to recv_data_srct->addr (cast to sockaddr *)
			NULL);

	return recved;

}
