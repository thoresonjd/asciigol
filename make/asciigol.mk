# asciigol.mk
# Author: Justin Thoreson
# Usage:
# - `make [asciigol]`: Builds the asciigol program

# Directories
INCLUDE_DIR = ./include
SRC_DIR = ./src
APP_DIR = ./main
OBJ_DIR = ./obj
OUT_DIR = ./bin
MAKE_DIR = ./make
MAKE_EXT = mk

# Program sources
ASCIIGOL = asciigol
PARSING = parsing

# C
C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0 -I$(INCLUDE_DIR)

$(ASCIIGOL): $(APP_DIR)/$(ASCIIGOL).c $(SRC_DIR)/$(ASCIIGOL).c $(OBJ_DIR)/$(PARSING).o
	$(C) $(C_FLAGS) $^ -o $(OUT_DIR)/$@

$(OBJ_DIR)/$(PARSING).o:
	make -f $(MAKE_DIR)/$(PARSING).$(MAKE_EXT)
