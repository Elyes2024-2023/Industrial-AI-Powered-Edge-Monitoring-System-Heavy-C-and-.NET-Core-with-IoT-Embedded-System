# Industrial AI-Powered Edge Monitoring System
# Copyright (c) ELYES 2024-2025. All rights reserved.

CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = -lm

# Build configuration
DEBUG ?= 0
PROFILE ?= 0
STATIC ?= 0

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LOG_DIR = logs

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Binary name
TARGET = $(BIN_DIR)/edgetrack

# Compiler flags
ifeq ($(DEBUG), 1)
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

ifeq ($(PROFILE), 1)
    CFLAGS += -pg
    LDFLAGS += -pg
endif

ifeq ($(STATIC), 1)
    LDFLAGS += -static
endif

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LOG_DIR)
	@echo "Industrial AI-Powered Edge Monitoring System - Done by ELYES"

# Link object files to create binary
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build completed successfully"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean completed"

# Run the program
run: all
	./$(TARGET)

# Build with debug information
debug: CFLAGS += -g -O0 -DDEBUG
debug: clean all

# Build with profiling information
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: clean all

# Build static binary
static: LDFLAGS += -static
static: clean all

# Help target
help:
	@echo "Industrial AI-Powered Edge Monitoring System - Build Options"
	@echo "Copyright (c) ELYES 2024-2025. All rights reserved."
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build the project (default)"
	@echo "  clean      - Remove build files"
	@echo "  run        - Build and run the program"
	@echo "  debug      - Build with debug information"
	@echo "  profile    - Build with profiling information"
	@echo "  static     - Build static binary"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  DEBUG=1    - Enable debug build"
	@echo "  PROFILE=1  - Enable profiling"
	@echo "  STATIC=1   - Build static binary"
	@echo ""
	@echo "Example: make debug"

.PHONY: all clean run debug profile static help directories 