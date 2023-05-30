OBJECTS += $(patsubst %.c, %.o, $(wildcard *.c))

all: $(OBJECTS) lz4dec.S.o

lz4dec.S.o: lz4dec.S
	$(CC) -c -nostdlibinc -I.. $(CFLAGS) lz4dec.S -o lz4dec.S.o

%.o: %.c
	$(CC) -c -nostdlibinc -I.. $(CFLAGS) $<

.PHONY: all
