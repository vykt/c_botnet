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

 
//TODO DEBUG INCLUDES, REMOVE
#include <stdio.h>

int main() {

//Make sock
int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (sock == -1) {printf("Create sock failed.\n"); return 1;}

//Set nonblock
int temp = fcntl(sock, F_SETFL, fcntl(
                 sock, F_GETFL, 0) | O_NONBLOCK);
if (temp == -1) {printf("Failed fnctl.\n"); return 1;}

//Make addr
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr("127.0.0.1");
addr.sin_port = htons(25000);

//Bind, Listen
bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
listen(sock, 1);

//Accept
int new_sock;
while (1) {

	sleep(1);
	socklen_t len = (socklen_t) sizeof(addr);
	new_sock = accept(sock, (struct sockaddr *)&addr, &len);
	printf("new_sock: %d\n", new_sock);
	printf("Still going for some reason...\n");
	if (new_sock != -1)  {

		int temp = fcntl(new_sock, F_SETFL, fcntl(
		                 new_sock, F_GETFL, 0) | O_NONBLOCK);
		if (temp == -1) {printf("Failed new fnctl.\n"); return 1;}
		break;
	}
}

//Get data
char buf[64];
ssize_t ret;
while (1) {

	sleep(1);

	memset(buf, 0, 64);
	
	ret = recv(new_sock, buf, 64, 0);
	printf("Ret = %d\n", ret);
	printf("Buf = %s\n", buf);

}
}
