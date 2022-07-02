#include <stddef.h>
#include <sys/time.h>

#include "error.h"


// 0 = do nothing, 1 = send
int check_send(struct timeval * tv_last_send, int init) {

	struct timeval tv_current;

	if (init) {
		if (gettimeofday(&tv_current, NULL) == -1) {
			handle_err(ERROR_TIME_GETTIME);
		}
		return 0;
	}

	if (gettimeofday(&tv_current, NULL) == -1) {
		handle_err(ERROR_TIME_GETTIME);
	}

	//If more than 4 to 5 seconds have elapsed
	if (tv_last_send->tv_sec + 5 < tv_current.tv_sec) {
		if (gettimeofday(tv_last_send, NULL) == -1) {
			handle_err(ERROR_TIME_GETTIME);
		}
		return 1;
	}

	return 0; 


}
