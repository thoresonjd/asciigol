# Makefile
# Author: Justin Thoreson
# Usage:
# - `make [all]`: Builds all programs
# - `make asciigol`: Builds the asciigol program
# - `make config_gen`: Builds the configuration file generator program

C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0
ASCIIGOL = asciigol
ASCIIGOLGEN = asciigolgen
PARSING = parsing
PROGRAMS = $(ASCIIGOL) $(ASCIIGOLGEN)

all: $(PROGRAMS)

$(ASCIIGOL): $(ASCIIGOL)_main.c $(ASCIIGOL).c $(PARSING).o
	$(C) $(C_FLAGS) $^ -o $@

$(ASCIIGOLGEN): $(ASCIIGOLGEN)_main.c $(ASCIIGOLGEN).c $(PARSING).o
	$(C) $(C_FLAGS) $^ -o $@

$(PARSING).o: $(PARSING).c
	$(C) $(C_FLAGS) $< -c -o $@

.PHONY: clean

clean:
	rm -f *.o $(PROGRAMS)
	
