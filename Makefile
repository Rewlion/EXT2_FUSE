CC = gcc
CFLAGS = -std=c11 -Wall -g
SOURCES = main.c endian_helpers.c ext2_helpers.c ext2_fuse.c
FUSE = `pkg-config fuse3 --cflags --libs`

OBJECTS = $(SOURCES:.c=.o)
EXE = ext2_fuse

ALL: $(SOURCES) $(EXE)

$(EXE) : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(FUSE)

$(OBJECTS) : %.o : %.c
	$(CC) $(CFLAGS) $(FUSE) -c $< -o $@

clean:
	rm *.o $(EXE)
