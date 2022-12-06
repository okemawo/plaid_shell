CFLAGS=-Wall -Werror -g
LIBS=-lreadline

all: plaidsh test

plaidsh: parser.o plaidsh.o
	gcc $(LDFLAGS) $^ $(LIBS) -o $@

test_parser: parser.o test_parser.o
	gcc $(LDFLAGS) $^ -o test_parser

test: test_parser
	./test_parser

%.o: %.c %.h
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o test_parser plaidsh
