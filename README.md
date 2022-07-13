# c_botnet


### ABOUT:

A cluser/botnet written in C without the use of external libraries. Both the dispatcher and host code runs on POSIX compliant environments that support BSD sockets. A REST API is provided to communicate with the dispatcher, written in Python & Flask. The project is configured to run inside docker by default, but a script is provided to convert the sources to run on localhost for testing.

Hosts are programmed to calculate whether a number passed to them appears in the fibonacci sequence. Communication between the dispatcher and the client is disguised to appear as an unsuccessful series of Gopher protocol requests. The real communication occurs inside the checksum field of the UDP header.



### INSTALLATION:

Build master & host:
```
cd master && make
cd ../host && make
cd ..
```

Build docker images:
```
sudo ./docker_build.sh
```


### RUN:

Launch docker:
```
sudo docker compose up
```


### INTERACT WITH API

Connections with the API are done on the following address:
```
127.0.0.1:5000
```

Data is passed to the API using the 'number' parameter on the 'post' page:
```
127.0.0.1:5000/post?number=[n]
```

The API will return a dictionary containing 3 fields:
```
"number"		- The number originally passed to the API.
"in_fibonacci"	- Whether the number is in the fibonacci sequence, or error.
"reason"		- None, or in case of error, reason for error.
```


### EXAMPLE

Check whether the number 21 is in the fibonacci sequence:
```
curl -X POST 127.0.0.1:5000/post?number=21
```


### FUTURE CONSIDERATIONS

While the Dispatcher to API communication is over TCP, Dispatcher to Host communication relies entirely on UDP, with completely unreliable data transfer. Should this cluster be expanded on to be something more practical transfer safety would need to be implemented, expanding on the little groundwork already laid.

Currently hosts sharing a public IP address are not distinguished between by the Dispatcher. This should be fixed by storing and checking not only addresses, but also ports which can be extracted from UDP headers.

While a queue for jobs is implemented, its not utilised beyond storing a single value. The dispatcher can be made to track which host is assigned which job, while the API can be made to send and receive non-sequentially. IDs can be attached to each request. Together, this would completely unlock the potential of the cluster.
