#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

int main() {

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(25000);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = connect(sock, (struct sockaddr *) &addr, sizeof(addr));
	
	char buf[8] = "15";

	//ssize_t ret_sent = send(sock, buf, sizeof(buf), 0);


	printf("%d\n", ret);

	perror("why? ");

	close(sock);

	return 0;
}
