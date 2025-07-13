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

$(ASCIIGOL): main.c $(ASCIIGOL).c
	$(C) $(C_FLAGS) $^ -o $@

$(CONFIG_GEN): $(CONFIG_GEN).c
	$(C) $(C_FLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(PROGRAMS)
	
