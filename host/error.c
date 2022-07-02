#include <stdlib.h>
#include <errno.h>
#include "error.h"

//TODO debug includes, remove
#include <stdio.h>

void handle_err(int err_id) {

	switch (err_id) {

		case ERROR_SOCKET_OPEN:
			perror("ERROR_SOCKET_OPEN");
			exit(err_id);
			break;
		
		case ERROR_SOCKET_NONBLOCK:
			perror("ERROR_SOCKET_NONBLOCK");
			exit(err_id);
			break;

		case ERROR_TIME_GETTIME:
			perror("ERROR_TIME_GETTIME");
			exit(err_id);
			break;
	}
}
