CC=gcc
CFLAGS=-I.
OBJ=blackhole.o crc32.o
DEPS=crc32.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

blackhole: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f blackhole *.o *~ *.bh *.map