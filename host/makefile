CC=gcc
CFLAGS=
INCLUDES=
LIBS=
CLEAN_TARGETS=test_elf host_elf fibonacci.o fibonacci.h.gch network.o network.h.gch error.o error.h.gch TEST.o


test: TEST.o fibonacci.o fibonacci.h network.o network.h error.o error.h
	${CC} ${CFLAGS} -o test_elf TEST.o fibonacci.o fibonacci.h ${LIBS}

TEST.o: TEST.c fibonacci.h
	${CC} ${CFLAGS} -c TEST.c fibonacci.h ${LIBS}

fibonacci.o: fibonacci.c fibonacci.h
	${CC} ${CFLAGS} -c fibonacci.c fibonacci.h ${LIBS}

network.o: network.c network.h
	${CC} ${CFLAGS} -c network.c network.h ${LIBS}

error.o: error.c error.h
	${CC} ${CFLAGS} -c error.c error.h ${LIBS}

clean:
	rm ${CLEAN_TARGETS}
