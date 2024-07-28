# Copyright Joan Montas
# All rights reserved.
# License under GNU General Public License v3.0

CC = gcc
CFLAGS = --std=gnu11 -Wextra -Wpedantic -g -Wformat -Wshadow -Wconversion -Wall -fsanitize=address
EXECUTABLE_NAME = sha_one

TEST_FRAMEWORK = -lcunit

all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): main.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE_NAME) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm ./*.o ./$(EXECUTABLE_NAME)