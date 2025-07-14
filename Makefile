# Makefile
# Author: Justin Thoreson
# Usage:
# - `make [all]`: Builds all programs
# - `make asciigol`: Builds the asciigol program
# - `make asciigolgen`: Builds the configuration file generator program
# - `make setup`: Creates the output build directories if they don't exist
# - `make clean`: Deletes the output build directories

OBJ_DIR = ./obj
OUT_DIR = ./bin
BUILD_DIRS = $(OBJ_DIR) $(OUT_DIR)
MAKE_DIR = ./make
MAKE_EXT = mk
PROGRAMS = asciigol asciigolgen

all: $(PROGRAMS)

setup:
	mkdir -p $(BUILD_DIRS)

clean:
	rm -rf $(BUILD_DIRS)

.PHONY: all setup clean

$(PROGRAMS): setup
	make -f $(MAKE_DIR)/$@.$(MAKE_EXT)

