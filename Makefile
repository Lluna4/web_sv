CC = g++

all : main.cpp
	$(CC) main.cpp

clean:
	rm -rf output/*.out

re : clean all

.PHONY: all clean re