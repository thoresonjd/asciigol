# asciigolgen.mk
# Author: Justin Thoreson
# Usage:
# - `make [asciigolgen]`: Builds the configuration file generator program

# Directories
INCLUDE_DIR = ./include
SRC_DIR = ./src
APP_DIR = ./main
OBJ_DIR = ./obj
OUT_DIR = ./bin
MAKE_DIR = ./make
MAKE_EXT = mk

# Program sources
ASCIIGOLGEN = asciigolgen
PARSING = parsing

# C
C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0 -I$(INCLUDE_DIR)

$(ASCIIGOLGEN): $(APP_DIR)/$(ASCIIGOLGEN).c $(SRC_DIR)/$(ASCIIGOLGEN).c $(OBJ_DIR)/$(PARSING).o
	$(C) $(C_FLAGS) $^ -o $(OUT_DIR)/$@

$(OBJ_DIR)/$(PARSING).o:
	make -f $(MAKE_DIR)/$(PARSING).$(MAKE_EXT)
