CC=gcc
CFLAGS=
INCLUDES=
LIBS=
CLEAN_TARGETS=main_elf fibonacci.o fibonacci.h.gch network.o network.h.gch error.o error.h.gch queue.o queue.h.gch main.o


main: main.o fibonacci.o fibonacci.h network.o network.h error.o error.h queue.o queue.h
	${CC} ${CFLAGS} -o main_elf main.o fibonacci.o fibonacci.h network.o network.h error.o error.h queue.o queue.h ${LIBS}

main.o: main.c fibonacci.h network.h
	${CC} ${CFLAGS} -c main.c fibonacci.h network.h ${LIBS}

fibonacci.o: fibonacci.c fibonacci.h
	${CC} ${CFLAGS} -c fibonacci.c fibonacci.h ${LIBS}

network.o: network.c network.h error.h
	${CC} ${CFLAGS} -c network.c network.h network.h ${LIBS}

error.o: error.c error.h
	${CC} ${CFLAGS} -c error.c error.h ${LIBS}

queue.o: queue.c queue.h
	${CC} ${CFLAGS} -c queue.c queue.h ${LIBS}

clean:
	rm ${CLEAN_TARGETS}
