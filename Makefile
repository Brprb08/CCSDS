# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build

# Executable names (now stored in build/)
TARGET = $(BUILD_DIR)/ccsds_demo
TEST_TARGET = $(BUILD_DIR)/run_tests

# Sources
SRCS = $(SRC_DIR)/ccsds.c main.c
OBJS = $(BUILD_DIR)/ccsds.o $(BUILD_DIR)/main.o

TEST_SRCS = $(SRC_DIR)/ccsds.c $(TEST_DIR)/test_ccsds.c
TEST_OBJS = $(BUILD_DIR)/ccsds.o $(BUILD_DIR)/test_ccsds.o

# Default target
all: $(TARGET)

# Build demo executable (in build/)
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Build test executable (in build/)
test: $(TEST_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $^
	./$(TEST_TARGET)

# Compile from src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile from tests/
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile main.c in root
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean just objects & binaries
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET) $(TEST_TARGET)

# Deep clean: wipe build folder entirely
distclean:
	rm -rf $(BUILD_DIR)

# Run the demo program from build/
run: $(TARGET)
	./$(TARGET) $(ARGS)
