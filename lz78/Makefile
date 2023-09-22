CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LDFLAGS = -lm
ENCODE_OBJS = encode.o trie.o word.o io.o
DECODE_OBJS = decode.o trie.o word.o io.o

all: encode decode

encode: $(ENCODE_OBJS)
	$(CC) $(LDFLAGS) -o encode $(ENCODE_OBJS)

decode: $(DECODE_OBJS)
	$(CC) $(LDFLAGS) -o decode $(DECODE_OBJS)

debug: CFLAGS+= -g -gdwarf-4
debug: encode decode

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f encode decode $(ENCODE_OBJS) $(DECODE_OBJS)

format:
	clang-format -i -style=file *.[ch]

scan-build: clean
	scan-build --use-cc=$(CC) make
