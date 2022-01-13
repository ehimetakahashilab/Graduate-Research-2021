CC = gcc
CFLAGS = -O3
OBJ = lfsr.o

PROG := lfsr

$(PROG):$(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ)

clean:
	rm -rf *.o *~

cleanall:
	rm -rf *.test *.o *~
