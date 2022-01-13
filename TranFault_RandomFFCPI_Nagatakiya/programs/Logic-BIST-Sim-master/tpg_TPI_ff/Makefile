CC = gcc
CFLAGS = -O3 
OBJ = lfsr.o

PROG := lfsr_ff

$(PROG):$(OBJ) 
	$(CC) -o $@ $(CFLAGS) $(OBJ)

clean:
	rm -f *.o *~

cleanall:
	rm -f *.test *.o *~
