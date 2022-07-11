#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


int main() {

	//Make sock
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (sock == -1) {printf("sock error.\n"); return 1;}
	int fcntl_ret = fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
	if (fcntl_ret == -1) {printf("fcntl error.\n"); return 1;}

	//Make addr
	struct sockaddr_in addr;

	//Make body
	char packet[148]; // = {0};

	//Make content
	struct iphdr * ip_header;
	struct udphdr * udp_header;
	char * body;

	//Receive
	while (1) {

		sleep(1);
	
		memset(packet, 0, 148);
		memset(&addr, 0, sizeof(struct sockaddr_in));

		struct iphdr * ip_header = (struct iphdr *) packet;
		struct udphdr * udp_header = (struct udphdr *) (ip_header + 1);
		char * body = (char *) (udp_header + 1);

		socklen_t len;

		ssize_t bytes = recvfrom(sock, packet, 148, 0, (struct sockaddr *)&addr, &len);
		printf("Recv: %ld, Checksum: %u\nBody: %s\n", bytes, ntohs(udp_header->check), body);


	}

	return 0;
}
