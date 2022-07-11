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

#include <stdio.h>

int main() {

	//Make sock
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {printf("Create sock failed.\n"); return 1;}

	//Make addr
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(25000);

	connect(sock, (struct sockaddr *)&addr, sizeof(addr));

	ssize_t sent;
	char buf[64];
	char * msg = "15";
	memset(buf, 0, 64);
	strcpy(buf, msg);

	while (1) {
		
		sleep(1);
		sent = send(sock, buf, 64, 0);
		printf("Sent: %ld\n", sent);
	}


	return 0;
}
