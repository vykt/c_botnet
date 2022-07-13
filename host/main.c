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

	char * master_ip = "172.20.0.2"; //Docker
	//char * master_ip = "127.0.0.1"; //Local

	int sock;
	ssize_t recv;
	int tms_check;
	uint16_t num_check;
	struct send_data send_data_srct;
	struct recv_data recv_data_srct;
	struct master_data master_data_srct;
	time_t time_last_send;

	srand(time(0));

	/*	In the future, I'd like to make the host program read info
	 *	about the dispatcher from elsewhere, rather than hardcoding
	 *	it into the program.
	 */

	strcpy(master_data_srct.ip, master_ip);
	//Generate port in the dynamic/private range. If 2 clients happen to
	//have clashing ports on same public IP, there will be a problem.
	master_data_srct.port = (rand() % 16384) + 49152;

	build_sock(&sock);
	build_send(&send_data_srct, &master_data_srct);
	build_master(&master_data_srct);
	build_recv(&recv_data_srct, &master_data_srct);
	check_send(&time_last_send, CHECK_INIT_TRUE);

	//Main loop
	while (1) {

		tms_check = check_send(&time_last_send, CHECK_INIT_FALSE);		
		recv = try_recv(&recv_data_srct, &master_data_srct, &sock);

		//If recv received some udp packet
		if (recv != -1) {
			num_check = ntohs(recv_data_srct.udp_header->check);
			num_check = fibonacci_calc(num_check);
			update_send(&send_data_srct, &master_data_srct, num_check);
			try_send(&send_data_srct, &master_data_srct, &sock);
		}

		//Check if ping time.
		if (tms_check == 1) {
			update_send(&send_data_srct, &master_data_srct, 0);
			try_send(&send_data_srct, &master_data_srct, &sock);
		}

	} //End main loop

	close(sock);
}
	


