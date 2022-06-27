CC=gcc
CFLAGS=
INCLUDES=
LIBS=
CLEAN_TARGETS=test_elf host_elf fibonacci.o test.o


test: TEST.o fibonacci.o
	${CC} ${CFLAGS} -o test_elf TEST.o fibonacci.o fibonacci.h ${LIBS}

TEST.o: TEST.c
	${CC} ${CFLAGS} TEST.c fibonacci.h ${LIBS}

fibonacci.o: fibonacci.c
	${CC} ${CFLAGS} fibonacci.c fibonacci.h ${LIBS}

clean:
	rm ${TARGETS}
