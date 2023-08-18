CC       = clang
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -gdwarf-4 -Ofast

.PHONY: clean spotless decode encode all

all: decode encode

decode: stack.o node.o heap_help.o pq.o code.o io.c huffman.o decode.o
	$(CC) -o $@ $^

encode: stack.o node.o heap_help.o pq.o code.o io.c huffman.o encode.o
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o

spotless:
	rm -f encode *.o

format:
	clang-format -i -style=file *.[c]

scan-build: clean
	scan-build --use-cc=$(CC) make
