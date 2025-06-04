/**
 * @file main.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include "asciigol.h"
#include <stdbool.h>
#include <string.h>

static void parseArgs(struct AsciigolArgs* const args, const int argc, char** const argv) {
	for (int i = 1; i < argc; i++)
		if (!strcmp("--wrap", argv[i]) && !args->wrapAround)
			args->wrapAround = true;
}

int main(int argc, char** argv) {
	struct AsciigolArgs args;
	parseArgs(&args, argc, argv);
	asciigol(&args);
	return 0;
}

