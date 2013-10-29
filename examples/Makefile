
EXAMPLES = keypress

all: clean $(EXAMPLES)

clean:
	rm -f *-example

%: %.c
	$(CC) $^ $(CFLAGS) -o $@-example

.PHONY: all clean
