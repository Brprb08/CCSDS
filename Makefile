# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude
LDFLAGS =

# Directories
SRC_DIR   = src
INC_DIR   = include
TEST_DIR  = tests
BUILD_DIR = build

# Binaries
TARGET      = $(BUILD_DIR)/ccsds_demo
TEST_TARGET = $(BUILD_DIR)/run_tests

# --- source discovery ---
SRC_CS     = $(wildcard $(SRC_DIR)/*.c)
APP_SRCS   = $(SRC_CS) main.c

# tests: list your new modular test files explicitly
TEST_ONLY_CS = $(TEST_DIR)/test_main.c \
               $(TEST_DIR)/test_primary_header.c \
               $(TEST_DIR)/test_secondary_header.c
TEST_SRCS    = $(SRC_CS) $(TEST_ONLY_CS)

# --- objects (mirror everything into build/) ---
APP_OBJS  = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_CS)) \
            $(BUILD_DIR)/main.o

TEST_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_CS)) \
            $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_ONLY_CS))

# Phony
.PHONY: all test clean distclean run

# Default
all: $(TARGET)

# App link
$(TARGET): $(APP_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Test link + run
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Pattern rules: compile to build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build dir exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the demo
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Cleaning
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET) $(TEST_TARGET)

distclean:
	rm -rf $(BUILD_DIR)

