# Makefile
# Author: Justin Thoreson
# Usage:
# - `make [asciigol]`: Builds the asciigol program 
# - `make run`: Builds and runs the asciigol program

C = gcc
C_FLAGS = -std=gnu11 -Wall -Werror -pedantic -ggdb -O0
PROGRAM = asciigol

$(PROGRAM): main.c $(PROGRAM).c
	$(C) $(C_FLAGS) $^ -o $@

run: $(PROGRAM)
	./$(PROGRAM)

.PHONY: clean

clean:
	rm -f $(PROGRAM)
	
