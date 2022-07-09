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

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	listen(sock, 4);

	socklen_t addr_len = sizeof(addr);

	int new_sock = accept(sock, (struct sockaddr *)&addr, &addr_len);

	char buf[8] = {0};
	memset(buf, 0, 8);
	
	recv(new_sock, buf, 8, 0);
	printf("%s\n", buf);

	close(sock);

	return 0;
}
