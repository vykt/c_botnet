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


/*             _____
 *           \"_   _"/
 *           |(>)-(<)|
 *        ../  " O "  \..
 *------""(((:-.,_,.-:)))""--------
 *
 *		
 */

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
	int hosts_available = 0;

	build_sock(&sock);
	build_send(&send_data_srct);
	queue_init(&jobs);

	//Setup API
	struct api_data api_data_srct;
	int sock_listen;
	int sock_api;
	int api_ret;
	int addr_match;

	build_api(&api_data_srct, &sock_listen);


	//Wait for API to connect
	while (1) {
		api_ret = api_accept_conn(&api_data_srct, &sock_listen, &sock_api);
		if (api_ret == API_CONN_SUCCESS) break;
	}

	//Main loop
	while (1) {

		//Check for input from api.
		num_buf = api_get_input(&sock_api, &api_data_srct);

		if (num_buf > 0) {
			queue_push(&jobs, num_buf);
		}

		//Check for numbers to process.
		if (queue_not_empty(&jobs)) {

			//For every connected host
			hosts_available = 0; //false
			for (int i = 0; i < hosts_add_index; i++) {
				if (hosts[i].state != 1) continue; //If unavailable, skip
				
				hosts_available = 1;

				//Send to first available host
				num_buf = queue_pop(&jobs);
				update_send(&send_data_srct, &hosts[i], num_buf);

				try_send(&send_data_srct, &hosts[i], &sock, num_buf);
				hosts[i].state = 2; //Set host as working
			}

			//If API request comes in, but there is no hosts to process it
			if (queue_not_empty(&jobs) && hosts_available == 0) {
				queue_pop(&jobs);
				//Tell API that no hosts available.
				api_send_output(&sock_api, &api_data_srct, 0);
			}
		}

		//Check for pings
		build_recv(&recv_data_srct);

		//If something received
		addr_match = 0;
		int recved;
		recved = try_recv(&recv_data_srct, &sock);
		if (recved != -1) { //If received

			//Check all known addresses
			for (int i = 0; i < hosts_add_index; i++) { //For all known addrs

				//If address matches
				if (addr_equal(&recv_data_srct.addr, &hosts[i].addr)) {

					//If its not an ack (aka result)
					if (ntohs(recv_data_srct.udp_header->check) > 0) {

						set_ack_time(&hosts[i]);
						api_send_output(&sock_api, &api_data_srct,
									ntohs(recv_data_srct.udp_header->check));
						hosts[i].state = 1; //Set pinging

					//If its an ack
					} else {
						set_ack_time(&hosts[i]);
					}

					addr_match = 1;	
				} //End if addr matches

			} //End for all known addrs

			//If address doesn't match
			if (addr_match == 0) {
				
				memcpy(&hosts[hosts_add_index].addr, &recv_data_srct.addr,
					   sizeof(struct sockaddr_in));

				set_ack_time(&hosts[hosts_add_index]);
				hosts[hosts_add_index].state = 1; //Set host as pinging
				hosts_add_index++;

			}
		} //End if received

		//Check outdated connections
		if (gettimeofday(&time_buf, NULL) != 0) handle_err(ERROR_TIME_GETTIME);
		
		for (int i = 0; i < hosts_add_index; i++) {
			//If outdated
			if (check_outdated_ack_time(&hosts[i], &time_buf) == 1
				&& hosts[i].state != 0) {
				
				hosts[i].state = 0; //Disconnected

			//Else not outdated
			} else if (check_outdated_ack_time(&hosts[i], &time_buf) == 0
					   && hosts[i].state == 0) {

				hosts[i].state = 1; //pinging
			}
		}
	} //End main loop
}
