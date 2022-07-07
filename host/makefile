CC=gcc
CFLAGS=-Wall
INCLUDES=
LIBS=
CLEAN_TARGETS=main_elf fibonacci.o fibonacci.h.gch network.o network.h.gch error.o error.h.gch manager.o manager.h.gch main.o


main: main.o fibonacci.o fibonacci.h network.o network.h error.o error.h manager.o manager.h
	${CC} ${CFLAGS} -o main_elf main.o fibonacci.o fibonacci.h network.o network.h error.o error.h manager.c manager.h ${LIBS}

main.o: main.c fibonacci.h network.h error.h manager.h
	${CC} ${CFLAGS} -c main.c fibonacci.h network.h error.h manager.h${LIBS}

fibonacci.o: fibonacci.c fibonacci.h
	${CC} ${CFLAGS} -c fibonacci.c fibonacci.h ${LIBS}

network.o: network.c network.h fibonacci.h error.h
	${CC} ${CFLAGS} -c network.c network.h fibonacci.h error.h ${LIBS}

error.o: error.c error.h
	${CC} ${CFLAGS} -c error.c error.h ${LIBS}

manager.o: manager.c manager.h
	${CC} ${CFLAGS} -c manager.c manager.h ${LIBS}

clean:
	rm ${CLEAN_TARGETS}
