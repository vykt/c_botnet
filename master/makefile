CC=gcc
CFLAGS=-Wall -ggdb
INCLUDES=
LIBS=
CLEAN_TARGETS=master_elf network.o network.h.gch error.o error.h.gch queue.o queue.h.gch util.o util.h.gch main.o


main: main.o network.o network.h error.o error.h queue.o queue.h util.o util.h
	${CC} ${CFLAGS} -o master_elf main.o network.o network.h error.o error.h queue.o queue.h util.o util.h ${LIBS}

main.o: main.c network.h error.h queue.h util.h
	${CC} ${CFLAGS} -c main.c network.h error.h queue.h util.h ${LIBS}

network.o: network.c network.h error.h
	${CC} ${CFLAGS} -c network.c network.h error.h ${LIBS}

error.o: error.c error.h
	${CC} ${CFLAGS} -c error.c error.h ${LIBS}

queue.o: queue.c queue.h
	${CC} ${CFLAGS} -c queue.c queue.h ${LIBS}

util.o: util.c util.h network.h
	${CC} ${CFLAGS} -c util.c util.h network.h ${LIBS}

clean:
	rm ${CLEAN_TARGETS}
