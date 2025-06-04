/**
 * @file main.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include "asciigol.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool parseArgs(struct AsciigolArgs* const args, const int argc, char** const argv) {
	const char CHAR_PARAM[] = "--char=";
	const char WRAP_PARAM[] = "--wrap";
	const size_t CHAR_PARAM_SIZE = sizeof(CHAR_PARAM) - 1;
	for (int i = 1; i < argc; i++) {
		char* const arg = argv[i];
		int res = -1;
		if (!strncmp(CHAR_PARAM, arg, CHAR_PARAM_SIZE) && !args->character) {
			res = sscanf(arg + CHAR_PARAM_SIZE, "%c", &args->character);
		} else if (!strcmp(WRAP_PARAM, arg) && !args->wrapAround) {
			args->wrapAround = true;
		}
		if (!res)
			return false;
	}
	return true;
}

int main(int argc, char** argv) {
	struct AsciigolArgs args;
	if (parseArgs(&args, argc, argv))
		asciigol(&args);
	return 0;
}

