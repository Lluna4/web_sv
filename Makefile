CC = g++

all : main.cpp
	$(CC) main.cpp -std=c++20

clean:
	rm -rf output/*.out

re : clean all

.PHONY: all clean re
