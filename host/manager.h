#ifndef MANAGER_H
#define MANAGER_H

#include <sys/time.h>

#define CHECK_INIT_TRUE 1
#define CHECK_INIT_FALSE 0

int check_send(time_t * last_send_time, int init);

#endif
