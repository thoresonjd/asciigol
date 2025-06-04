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
	const char WIDTH_PARAM[] = "--width=";
	const char HEIGHT_PARAM[] = "--height=";
	const char CHAR_PARAM[] = "--char=";
	const char WRAP_PARAM[] = "--wrap";
	const size_t WIDTH_PARAM_SIZE = sizeof(WIDTH_PARAM) - 1;
	const size_t HEIGHT_PARAM_SIZE = sizeof(HEIGHT_PARAM) - 1;
	const size_t CHAR_PARAM_SIZE = sizeof(CHAR_PARAM) - 1;
	for (int i = 1; i < argc; i++) {
		char* const arg = argv[i];
		int res = -1;
		if (!strncmp(WIDTH_PARAM, arg, WIDTH_PARAM_SIZE) && !args->height) {
			// TODO: parsing uint directly is problematic: negative input will
			//       wrap around to the upper end of the uint range
			res = sscanf(arg + WIDTH_PARAM_SIZE, "%u", &args->width);
		} else if (!strncmp(HEIGHT_PARAM, arg, HEIGHT_PARAM_SIZE) && !args->height) {
			res = sscanf(arg + HEIGHT_PARAM_SIZE, "%u", &args->height);
		} else if (!strncmp(CHAR_PARAM, arg, CHAR_PARAM_SIZE) && !args->character) {
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
		asciigol(args);
	return 0;
}

