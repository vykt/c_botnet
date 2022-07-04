#include <netinet/in.h>
#include <string.h>

#include "network.h"

// 1 = equal, 0 = unequal
int addr_equal(struct sockaddr_in * addr_A, struct sockaddr_in * addr_B) {

	if (addr_A->sin_addr.s_addr == addr_B->sin_addr.s_addr) return 1;

}

void reset_addr(struct sockaddr_in * addr) {

	memset(addr, 0, sizeof(struct sockaddr_in));

}
