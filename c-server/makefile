CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = server.c
BUILD_DIR = build
OUTPUT = $(BUILD_DIR)/server

.PHONY: all clean run build

all: build

build: $(OUTPUT)

$(OUTPUT): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

run: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -rf $(BUILD_DIR)
