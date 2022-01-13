CC = gcc
CFLAGS = -O3 -Wno-unused-result
SOURCE_DIR = ./src

DEFFILE = $(SOURCE_DIR)/declare.h $(SOURCE_DIR)/def_flt.h $(SOURCE_DIR)/def_gtype.h
OBJ = $(SOURCE_DIR)/flist.o $(SOURCE_DIR)/main.o $(SOURCE_DIR)/nsort.o $(SOURCE_DIR)/fsim.o $(SOURCE_DIR)/inject.o $(SOURCE_DIR)/lfsr.o $(SOURCE_DIR)/Option_Set.o $(SOURCE_DIR)/sim_lib.o $(SOURCE_DIR)/flt_simtor.o

lbistsim : $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) -lm

clean:
	rm -f $(SOURCE_DIR)/*.o $(SOURCE_DIR)/*~ ./*~ ./lbistsim
