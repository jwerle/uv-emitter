
CFLAGS = -std=c99 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -luv

all: clean test

clean:
	rm -f emitter-test

test:
	$(CC) test.c -$(CFLAGS) -o emitter-test
	./emitter-test

examples:
	cd ./examples && $(MAKE)

.PHONY: all clean test examples
