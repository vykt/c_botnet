#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "network.h"
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
void build_send(struct send_data * send_data_srct) {

	//Write packet
	send_data_srct->packet_send_body = 
		(char *) (send_data_srct->packet_send + sizeof(struct udphdr));
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
				 uint16_t num_to_check) {
	int content_index;
	if (num_to_check > 0) {content_index = 1;} else {content_index = 0;}
	char * body_content[2] = {
		"<!DOCTYPE html>\n<header>\n</header>\n<body>\n<h1>Error 404</h1>\n</body>",
		"<!DOCTYPE html>\n<header>\n</header>\n<body>\n<h1>Error 403</h1>\n</body>"
	};

	//Fill body.
	send_data_srct->udp_header->dest = host_data_srct->addr.sin_port;
	memset(send_data_srct->packet_send, 0, sizeof(send_data_srct->packet_send));
	strncpy(send_data_srct->packet_send_body, body_content[content_index],
			strlen(body_content[content_index]));

	//Set ack or number to check. 0 = ack.
	send_data_srct->udp_header->check = num_to_check;

}


// Build api listener.
void build_api(struct api_data * api_data_srct, int * sock_listen) {

	//Create TCP socket
	if ((*sock_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		handle_err(ERROR_SOCKET_OPEN);
	}

	//Create master addr
	api_data_srct->addr_listen.sin_family = AF_INET;
	api_data_srct->addr_listen.sin_addr.s_addr = inet_addr("0.0.0.0");
	api_data_srct->addr_listen.sin_port = htons(PORT_API);

	//Set socket to not block
	int temp = fcntl(*sock_listen, F_SETFL, fcntl(
					 *sock_listen, F_GETFL, 0) | O_NONBLOCK);
	if (temp == -1) handle_err(ERROR_SOCKET_NONBLOCK);

	//Bind socket to listen for any addr, on PORT_API
	if (bind(*sock_listen, (struct sockaddr *) &api_data_srct->addr_listen,
			 sizeof(api_data_srct->addr_listen)) == -1) {

		handle_err(ERROR_API_LISTEN);
	}

	if (listen(*sock_listen, 1) == -1) {
		handle_err(ERROR_API_LISTEN);
	}
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
int try_send(struct send_data * send_data_srct, struct host_data * host_data_srct, 
			 int * sock, uint16_t num_to_check) {

	send_data_srct->udp_header->dest = htons(host_data_srct->addr.sin_port);
	send_data_srct->udp_header->check = num_to_check;

	ssize_t sent = sendto(*sock, 
						  send_data_srct->packet_send,
						  (sizeof(struct udphdr)+strlen(send_data_srct->packet_send_body)+1),
						  0,
						  (struct sockaddr *) &host_data_srct->addr,
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


// Update ack time
void set_ack_time(struct host_data * host) {

	if (gettimeofday(&host->ack_time, NULL) != 0) handle_err(ERROR_TIME_GETTIME);

}


/*
 * The program assumes UDP packets wont be dropped n number of times in a row,
 * in the current use case, 3 times in a row. If they are, the program assumes
 * host has lost connection. Connection is immediately deemed reistablished however
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


// Accept api connection
int api_accept_conn(struct api_data * api_data_srct, int * sock_listen,
					int * sock_api) {

	socklen_t len = (socklen_t) sizeof(api_data_srct);

	*sock_api = accept(*sock_listen, (struct sockaddr *) &api_data_srct->addr_api,
					   &len);

	if (*sock_api == -1) return API_CONN_FAIL;
	return API_CONN_SUCCESS;
}

// Get input from api. 0 = None, n = Input (number received)
int api_get_input(int * sock_api, struct api_data * api_data_srct) {
	
	ssize_t ret;
	char ** strtol_endptr = {0};
	memset(api_data_srct->ret_buf, 0, API_GET_SIZE);
	
	ret = recv(*sock_api, api_data_srct->ret_buf, API_GET_SIZE, 0);
	//If nothing to get
	if (ret == -1) {
		return 0;
	//If conn shutdown
	} else if (ret == 0) {
		/*
		 *	In future for reliability, what happens during shutdown
		 *	should be changed.
		 */
		return 0;
	//If something was received 
	} else if (ret > 0) {
		return (uint16_t) strtol(api_data_srct->ret_buf, strtol_endptr, 10);
	}

	return 0;
}

// Send output back to api
int api_send_output(int * sock_api, struct api_data * api_data_srct, 
					uint16_t in_fibonacci) {

	int ret;
	char * transfer_arr[3] = {"Invalid", "False", "True"};
	memset(api_data_srct->ret_buf, 0, API_GET_SIZE);
	memcpy(api_data_srct->ret_buf, transfer_arr[(int) in_fibonacci],
		   API_GET_SIZE);
	ret = send(*sock_api, api_data_srct->ret_buf, API_GET_SIZE, 0);
	
	//If send failed
	if (ret <= 0) {
		handle_err(ERROR_API_SEND);
	//Else if send succeeded
	}
	return 0;
}
