#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include "network.h"
#include "error.h"
#include "queue.h"
#include "util.h"


void sysmsg(char * msg) {

	printf("MASTER CONTROL: %s\n", msg);
}


int main() {

	//Setup
	struct send_data send_data_srct;
	struct recv_data recv_data_srct;
	
	int sock = 0;
	struct queue jobs;
	struct host_data hosts[MAX_HOST];
	struct timeval time_buf;
	int hosts_add_index = 0;
	uint16_t num_buf;

	build_send(&send_data_srct);
	queue_init(&jobs);

	//Setup API
	struct api_data api_data_srct;
	int sock_listen;
	int sock_api;
	int api_ret;

	build_api(&api_data_srct, &sock_listen);

	sysmsg("Setup complete.");

	//Wait for API to connect
	while (1) {
		api_ret = api_accept_conn(&api_data_srct, &sock_listen, &sock_api);
		if (api_ret == API_CONN_SUCCESS) break;
		sleep(1);
	}

	sysmsg("API connected.");

	//Main loop
	while (1) {

		//TODO remove
		sleep(1);

		//Check for input from api.
		num_buf = api_get_input(&sock_api, &api_data_srct);
		printf("VALUE: num_buf=%d\n", num_buf);


		if (num_buf > 0) {
			queue_push(&jobs, num_buf);
			sysmsg("Number received from API.");
		}

		//Check for numbers to process.
		if (queue_not_empty(&jobs)) {

			//For every connected host
			for (int i = 0; i < hosts_add_index; i++) {
				if (hosts[i].state != 1) continue; //if unavailable, skip
				//Send to first available host
				num_buf = queue_pop(&jobs);
				update_send(&send_data_srct, &hosts[i], num_buf);
				try_send(&send_data_srct, &hosts[i], &sock, num_buf);
				hosts[i].state = 2; //working
			}

		}

		//Check for pings
		build_recv(&recv_data_srct);

		//If something received
		if (try_recv(&recv_data_srct, &sock) != -1) {
		
			//Check all known addresses
			for (int i = 0; i < hosts_add_index; i++) {
				
				//If address matches
				if (addr_equal(&recv_data_srct.addr, &hosts[i].addr)) {
					
					//If its not an ack (aka result)
					if (recv_data_srct.udp_header->check > 0) {

						//Do what?
						set_ack_time(&hosts[i]);
						api_send_output(&sock_api, &api_data_srct, 
										recv_data_srct.udp_header->check);
						hosts[i].state = 1; //set pinging

						sysmsg("Received result.");

					//If its an ack
					} else {
					
						sysmsg("Received ack.");

						//Do what?
						set_ack_time(&hosts[i]);

					}
				
				//If address doesn't match
				} else {

					//Do what?
					//-record new addr
					memcpy(&hosts[hosts_add_index], &recv_data_srct.addr,
						   sizeof(struct host_data));
					hosts_add_index++;
					set_ack_time(&hosts[i]);

					sysmsg("Recorded new host.");
				}
			}

		//If nothing received
		} else {
			//do nothing

		}
		//Check outdated connections
		if (gettimeofday(&time_buf, NULL) != 0) handle_err(ERROR_TIME_GETTIME);
		for (int i = 0; i < hosts_add_index; i++) {
			//If outdated
			if (check_outdated_ack_time(&hosts[i], &time_buf) == 1) {
				hosts[i].state = 0; //disconnected	
		
				sysmsg("Host disconnected.");

			//Else not outdated
			} else {
				hosts[i].state = 1; //pinging
			}
		}
		
	} //End main loop


}
