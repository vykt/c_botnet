#ifndef ERROR_H
#define ERROR_H

//error codes
#define ERROR_SOCKET_OPEN 1
#define ERROR_SOCKET_NONBLOCK 2
#define ERROR_TIME_GETTIME 3
#define ERROR_API_LISTEN 4
#define ERROR_API_SEND 5

void handle_err(int err_id);

#endif
