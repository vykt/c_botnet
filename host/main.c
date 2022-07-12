#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "fibonacci.h"
#include "network.h"
#include "error.h"
#include "manager.h"



int main() {

	char * master_ip = "127.0.0.1";

	int sock;
	ssize_t sent;
	ssize_t recv;
	int tms_check;
	uint16_t num_check;
	struct send_data send_data_srct;
	struct recv_data recv_data_srct;
	struct master_data master_data_srct;
	time_t time_last_send;

	srand(time(0));

	strcpy(master_data_srct.ip, master_ip); //TODO temp, change
	//Generate port in the dynamic/private range. If 2 clients happen to
	//have clashing ports there will be a problem.
	master_data_srct.port = (rand() % 16384) + 49152;
	printf("HOST: Port chosen - %d\n", master_data_srct.port); 


	build_sock(&sock);
	build_send(&send_data_srct, &master_data_srct);
	build_master(&master_data_srct);
	build_recv(&recv_data_srct, &master_data_srct);
	check_send(&time_last_send, CHECK_INIT_TRUE);

	//Main loop
	while (1) {

		//sleep(1);

		printf(" --- NEW HOST CYCLE --- \n");

		printf(" - RECEIVE\n");
		tms_check = check_send(&time_last_send, CHECK_INIT_FALSE);	
		printf("time to send: %d\n", tms_check);
		
		recv = try_recv(&recv_data_srct, &master_data_srct, &sock);
		printf("received bytes: %ld\n", recv);	

		//If recv received some udp packet
		if (recv != -1) {
			num_check = ntohs(recv_data_srct.udp_header->check);
			printf("num to check: %u\n", num_check);
			num_check = fibonacci_calc(num_check);
			printf("fibonacci check (true/false): %u\n", num_check);
			update_send(&send_data_srct, &master_data_srct, num_check);
			sent = try_send(&send_data_srct, &master_data_srct, &sock);
			printf("sent calculation bytes: %ld\n", sent);

		}

		printf(" - SEND\n");
		//Check if ping time.
		if (tms_check == 1) {
			printf("asked to send.\n");
			update_send(&send_data_srct, &master_data_srct, 0);
			sent = try_send(&send_data_srct, &master_data_srct, &sock);
			printf("sent bytes: %ld\n", sent);
		}

		printf(" --- END HOST CYCLE --- \n\n\n");
	} //End main loop

	close(sock);

}
	


