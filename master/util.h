#ifndef UTIL_H
#define UTIL_H

#include <netinet/in.h>

int addr_equal(struct sockaddr_in * addr_A, struct sockaddr_in * addr_B);

#endif
