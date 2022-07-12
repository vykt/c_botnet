#include <stddef.h>
#include <time.h>
//#include <sys/time.h>

#include "error.h"

//debug
#include <stdio.h>

// 0 = do nothing, 1 = send
int check_send(time_t * last_send_time, int init) {

	time_t time_current;

	if (init == 1) {
		*last_send_time = time(NULL);
		return 0;
	}

	time_current = time(NULL);

	//If more than 4 to 5 seconds have elapsed
	
	//TODO TODO DEBUG TODO TODO

	// TODO END TODO
	
	
	if (*last_send_time + 5 < time_current) {
		*last_send_time = time_current;
		return 1;
	}

	return 0; 


}
