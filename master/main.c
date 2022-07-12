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
	ssize_t sent;
	int hosts_available = 0; // true/false

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


	//Wait for API to connect //WORKS
	while (1) {
		api_ret = api_accept_conn(&api_data_srct, &sock_listen, &sock_api);
		if (api_ret == API_CONN_SUCCESS) break;
		sleep(1);
	}
	//printf("API: Connected.\n\n");

	//Main loop
	while (1) {

		//sleep(1);

		//TODO remove
		printf(" --- NEW CYCLE --- \n");

		printf(" - API RECEIVING:\n");
		//Check for input from api.
		num_buf = api_get_input(&sock_api, &api_data_srct);
		printf("number from API: %u\n", num_buf);

		if (num_buf > 0) {
			queue_push(&jobs, num_buf);
		}

		//Check for numbers to process.
		printf(" - QUEUE:\n");
		if (queue_not_empty(&jobs)) {
			//printf("queue not empty: true\n");

			//For every connected host
			hosts_available = 0; //false
			for (int i = 0; i < hosts_add_index; i++) {
				printf("checking host %d.\n", i);
				if (hosts[i].state != 1) continue; //if unavailable, skip
				
				hosts_available = 1; //true
				printf("host %d is pinging.\n", i);
				//Send to first available host
				num_buf = queue_pop(&jobs);
				printf("popped number for processing: %u\n", num_buf);
				update_send(&send_data_srct, &hosts[i], num_buf);
				sent = try_send(&send_data_srct, &hosts[i], &sock, num_buf);
				printf("sent bytes: %ld\n", sent);
				printf("setting host %d as working.\n", i);
				hosts[i].state = 2; //working
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


		printf(" - HOSTS RECEIVING\n");
		//If something received
		addr_match = 0;
		int recved;
		recved = try_recv(&recv_data_srct, &sock);
		if (recved != -1) { //If received
		
			printf("received bytes: %d\n", recved);

			//Check all known addresses
			for (int i = 0; i < hosts_add_index; i++) { //For all known addrs
				
				printf("checking known host, id: %d\n", i);

				//If address matches
				if (addr_equal(&recv_data_srct.addr, &hosts[i].addr)) {
				
					printf("host matched, id: %d\n", i);

					//If its not an ack (aka result)
					if (ntohs(recv_data_srct.udp_header->check) > 0) {

						printf("received value was true/false: %u\n", 
							   ntohs(recv_data_srct.udp_header->check));

						//Do what?
						set_ack_time(&hosts[i]);
						printf("SET ACK TIME FOR CONNECTION %d\n", i);
						api_send_output(&sock_api, &api_data_srct, //segfault 
										ntohs(recv_data_srct.udp_header->check));
						printf("RESPONDED TO API\n");
						hosts[i].state = 1; //set pinging
						printf("SET HOST STATE\n");


					//If its an ack
					} else {
				
						printf("received value was ack: %u\n", 
							   ntohs(recv_data_srct.udp_header->check));

						//Do what?
						set_ack_time(&hosts[i]);
					}

					addr_match = 1;	
				} //End if addr matches

			} //End for all known addrs

			//If address doesn't match
			if (addr_match == 0) {

				printf("\nno address matched.\n");
				
				//Do what?
				memcpy(&hosts[hosts_add_index].addr, &recv_data_srct.addr,
					   sizeof(struct sockaddr_in));
				set_ack_time(&hosts[hosts_add_index]);
				hosts[hosts_add_index].state = 1; //pinging
				printf("recorded new address as index: %d\n", hosts_add_index);
				hosts_add_index++;

			}
		} //End if received

		printf(" - OUTDATED CONNS\n");
		//Check outdated connections
		if (gettimeofday(&time_buf, NULL) != 0) handle_err(ERROR_TIME_GETTIME);
		
		for (int i = 0; i < hosts_add_index; i++) {
			printf("checking if host is outdated, id: %d\n", i);
			//If outdated
			if (check_outdated_ack_time(&hosts[i], &time_buf) == 1
				&& hosts[i].state != 0) {
				
				printf("host deemed outdated (disconnected), id: %d\n", i);
				hosts[i].state = 0; //disconnected

			//Else not outdated
			} else if (check_outdated_ack_time(&hosts[i], &time_buf) == 0
					   && hosts[i].state == 0) {

				printf("host deemed to return to connection, id: %d\n", i);
				hosts[i].state = 1; //pinging
			}
			printf("\n");
		}
		printf(" --- END CYCLE --- \n\n\n");
	} //End main loop
}
