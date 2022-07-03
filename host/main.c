#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "fibonacci.h"
#include "network.h"
#include "manager.h"


int main() {

	//Initialise data
	
	srand(time(0));
	
	struct pass_data pass_data_host;
	strcpy(pass_data_host.ip, "172.20.10.10");
	//This creates a port number in the dynamic/private range (49152 to 65535)
	pass_data_host.port = (rand() % 16384) + 49152;

	struct conn_data conn_data_host;
	struct recv_data recv_data_host;

	struct timeval tv_last;
	check_send(&tv_last, CHECK_INIT_TRUE);
	
	int tms_check;
	uint16_t num_check;
	ssize_t recv;


	//Building data
	build_conn(&conn_data_host, &pass_data_host);
	build_recv(&recv_data_host, &conn_data_host);


	//Main loop
	while(1) {

		tms_check = check_send(&tv_last, CHECK_INIT_FALSE);
		
		recv = try_recv(&recv_data_host);
		//if recv received some udp packet
		if (recv > 0) {
			num_check = recv_data_host.packet_recv_check->check;
			num_check = fibonacci_calc(num_check);
			conn_data_host.udp_header->check = (uint16_t) num_check;
		} else {
			conn_data_host.udp_header->check = 2;
		}

		//If its time to send packet
		if (tms_check == 1) {
			try_send(&conn_data_host);
		}
	}

	close(conn_data_host.sock);
}
