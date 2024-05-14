CC = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

.PHONY: all clean run build

all: build

build: $(BUILD_DIR)/server

debug: CFLAGS += -g
debug: build

$(BUILD_DIR)/server: server.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

run: $(BUILD_DIR)/server
	./$(BUILD_DIR)/server

clean:
	rm -rf $(BUILD_DIR)
