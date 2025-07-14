# Makefile
# Author: Justin Thoreson
# Usage:
# - `make [all]`: Builds all programs
# - `make asciigol`: Builds the asciigol program
# - `make asciigolgen`: Builds the configuration file generator program

# Directories
INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
OUT_DIR = ./bin
BUILD_DIRS = $(OBJ_DIR) $(OUT_DIR)

# Program sources
ASCIIGOL = asciigol
ASCIIGOLGEN = asciigolgen
PARSING = parsing
PROGRAMS = $(ASCIIGOL) $(ASCIIGOLGEN)

# C
C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0 -I$(INCLUDE_DIR)

all: setup $(PROGRAMS)

setup:
	mkdir -p $(BUILD_DIRS)

clean:
	rm -rf $(BUILD_DIRS)

.PHONY: all setup clean

$(ASCIIGOL): $(ASCIIGOL)_main.c $(SRC_DIR)/$(ASCIIGOL).c $(OBJ_DIR)/$(PARSING).o
	$(C) $(C_FLAGS) $^ -o $(OUT_DIR)/$@

$(ASCIIGOLGEN): $(ASCIIGOLGEN)_main.c $(SRC_DIR)/$(ASCIIGOLGEN).c $(OBJ_DIR)/$(PARSING).o
	$(C) $(C_FLAGS) $^ -o $(OUT_DIR)/$@

$(OBJ_DIR)/$(PARSING).o: $(SRC_DIR)/$(PARSING).c
	$(C) $(C_FLAGS) $< -c -o $@

