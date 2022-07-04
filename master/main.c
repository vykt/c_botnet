#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include "fibonacci.h"
#include "network.h"
#include "manager.h"
#include "queue.h"
#include "util.h"


/*
 *	Need one socket;
 *
 *	One recv address;
 *
 *  Many send addresses;
 *  	> for every new address, add entry
 *
 */

int main() {

	//Define array of host structs

	//Build send packet
	//Build recv packet
	
	//Try get msg from recv
	//		>if got one, add address
	//See if there are any calculations in the queue
	//		>if no, go back to start
	//
	//Go through array of hosts, see if any available
	//		>if yes, send number to host

	//Setup
	struct host_data host_data_arr[MAX_HOST] = {0};
	struct send_data send_data_srct;
	struct recv_data recv_data_srct;
	
	int sock = 0;
	struct queue jobs;
	struct host_data hosts[MAX_HOST];
	struct sockaddr_in addr_buf;
	struct timeval time_buf;
	int hosts_add_index = 0;
	uint16_t num_buf;

	build_send(send_data_srct);
	queue_init(jobs);

	//Main loop
	while (1) {
		
		//Check for input from api.
		num_buf = (uint16_t) api_get_input();
		if (num_buf > 0) {
			queue_push(&jobs, num_buf);

		}

		//Check for numbers to process.
		if (queue_not_empty) {

			//For every connected host
			for (int i = 0; i < hosts_add_index; i++) {
				if (hosts[i].state != 1) continue; //if unavailable, skip
				//Send to first available host
				num_buf = queue_pop(&jobs);
				try_send(send_data_srct, &hosts[i], num_buf);
				hosts[i].state = 2; //working
			}

		}

		//Check for pings
		build_recv(recv_data_srct);
		reset_addr(&addr_buf);

		//If something received
		if (try_recv(recv_data_srct, &addr_buf) != -1) {
		
			//Check all known addresses
			for (int i = 0; i < host_add_index; i++) {
				
				//If address matches
				if (addr_equal(&addr_buf, hosts[i]->addr.sin_addr.s_addr)) {
					
					//If its not an ack (aka result)
					if (recv_data_srct.udp_header->check > 0) {

						//Do what?
						set_ack_time(hosts[i]);
						api_send_output(recv_data_srct.udp_header->check);
						hosts[i].state = 1; //set pinging

					//If its an ack
					} else {
						
						//Do what?
						set_ack_time(hosts[i]);

					}
				
				//If address doesn't match
				} else {

					//Do what?
					//-record new addr
					memcpy(, &hosts[hosts_add_index], sizeof(struct host_data));
					hosts_add_index++;
					set_ack_time(hosts[i]);
				}
			}

		//If nothing received
		} else {
			//do nothing

		}
		//Check outdated connections
		if (gettimeofday(&time_buf, NULL) != 0) handle_err(ERROR_TIME_GETTIME);
		for (int i = 0; i < host_add_index; i++) {
			//If outdated
			if (check_outdated_ack(&hosts[i], &time_buf) == 1) {
				hosts[i].state = 0; //disconnected	
			
			//Else not outdated
			} else {
				hosts[i].state = 1; //pinging
			}
		}
		
	} //End main loop


}
