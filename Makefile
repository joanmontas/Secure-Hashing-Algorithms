CC = gcc
CFLAGS = --std=gnu11 -Wextra -Wpedantic -g -Wformat -Wshadow -Wconversion -Wall -fsanitize=address
LDFLAGS = -lcunit
EXECUTABLE_NAME = sha_hash
BUILD_DIR = build
INCLUDE = include

all: $(BUILD_DIR) $(EXECUTABLE_NAME)

$(BUILD_DIR):
	mkdir -p $@

$(EXECUTABLE_NAME): $(BUILD_DIR)/main.o $(BUILD_DIR)/sha.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./bin/$(EXECUTABLE_NAME) $(BUILD_DIR)/main.o $(BUILD_DIR)/sha.o

$(BUILD_DIR)/main.o: src/main.c
	$(CC) $(CFLAGS) -I $(INCLUDE) -c $< -o $@

$(BUILD_DIR)/sha.o: src/sha.c
	$(CC) $(CFLAGS) -I $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) ./bin/$(EXECUTABLE_NAME)
