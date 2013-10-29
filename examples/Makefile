
CFLAGS = -std=c99 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -luv -I..
EXAMPLES = keypress

all: clean $(EXAMPLES)

clean:
	rm -f *-example

%: %.c
	$(CC) $^ $(CFLAGS) -o $@-example

.PHONY: all clean
