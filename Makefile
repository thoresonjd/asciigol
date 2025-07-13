# Makefile
# Author: Justin Thoreson
# Usage:
# - `make [all]`: Builds all programs
# - `make asciigol`: Builds the asciigol program
# - `make config_gen`: Builds the configuration file generator program

C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0
ASCIIGOL = asciigol
CONFIG_GEN = config_gen
PROGRAMS = $(ASCIIGOL) $(CONFIG_GEN)

all: $(PROGRAMS)

$(ASCIIGOL): $(ASCIIGOL)_main.c $(ASCIIGOL).c parsing.o
	$(C) $(C_FLAGS) $^ -o $@

$(CONFIG_GEN): $(CONFIG_GEN).c parsing.o
	$(C) $(C_FLAGS) $^ -o $@

parsing.o: parsing.c
	$(C) $(C_FLAGS) $< -c -o $@

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)
	
