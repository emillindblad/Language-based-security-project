CC = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

.PHONY: all clean run build

all: build

build: $(BUILD_DIR)/server-multi

$(BUILD_DIR)/server-multi: server-multi.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

run-multi: $(BUILD_DIR)/server-multi
	./$(BUILD_DIR)/server-multi

clean:
	rm -rf $(BUILD_DIR)
