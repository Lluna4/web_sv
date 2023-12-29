CC = gcc

all : main.c
	$(CC) main.c

clean:
	rm -rf output/*.out

re : clean all

.PHONY: all clean re