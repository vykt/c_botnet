#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

int main() {

	//Make sock
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sock == -1) {printf("sock fail\n"); return -1;}

	//Make addr
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(80);
	addr.sin_family = AF_INET;

	//Make body
	char packet[128] = {0};
	struct udphdr * udp_header = (struct udphdr *) packet;
	char * body = (char *) (udp_header + 1);
	memcpy(body, "test", 4);

	//Make udp header
	udp_header->source = htons(25252);
	udp_header->dest = htons(80);
	udp_header->len = htons(128);
	udp_header->check = htons(4);

	//Send
	ssize_t bytes;
	while (1) {

		sleep(1);
		bytes = sendto(sock, packet, 128, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));	
		printf("Sent %ld bytes.\n", bytes);
	}

	return 0;
}
