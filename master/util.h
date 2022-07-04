#ifndef UTIL_H
#define UTIL_H

#include <netinet_in.h>

int addr_equal(struct sockaddr_in * addr_A, struct sockaddr_in * addr_B);
void reset_addr(struct sockaddr_in * addr);

#endif
