
CFLAGS = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -luv -pthread
LD_LIBRARY_PATH = /usr/local/lib
export CFLAGS
export LD_LIBRARY_PATH

all: clean test

clean:
	rm -f emitter-test
	cd ./examples && $(MAKE) clean

test:
	$(CC) test.c $(CFLAGS) -o emitter-test
	./emitter-test

examples:
	cd ./examples && $(MAKE)

.PHONY: all clean test examples
