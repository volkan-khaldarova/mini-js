CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -Isrc/scanner -Isrc/value

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Main Application
APP_TARGET = aura
APP_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/scanner/scanner.c $(SRC_DIR)/value/value.c
# Flatten object files to obj/ directory
APP_OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/scanner.o $(OBJ_DIR)/value.o

# Phony Targets
.PHONY: all clean directories

# Default target: Build the main application
all: directories $(APP_TARGET)

# Link Main Application
$(APP_TARGET): $(APP_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile Source Files
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/scanner.o: $(SRC_DIR)/scanner/scanner.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/value.o: $(SRC_DIR)/value/value.c
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(APP_TARGET) $(APP_TARGET).exe
