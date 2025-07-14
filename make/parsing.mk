# parsing.mk
# Author: Justin Thoreson
# `make [obj/parsing.o]`: Build the object file for parsing

# Directories
INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj

# Program sources
PARSING = parsing

# C
C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0 -I$(INCLUDE_DIR)

$(OBJ_DIR)/$(PARSING).o: $(SRC_DIR)/$(PARSING).c
	$(C) $(C_FLAGS) -c $< -o $@

