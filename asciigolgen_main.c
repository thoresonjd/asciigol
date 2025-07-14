/**
 * @file asciigolgen_main.c
 * @brief Configuration file generator for asciigol.
 * @author Justin Thoreson
 * @date 2025
 */

#include <asciigolgen.h>
#include <parsing.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static const char* USAGE = "Usage: asciigolgen <filename> <width> <height>";

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("%s\n", USAGE);
		return EXIT_FAILURE;
	}
	uint8_t width, height;
	if (!parse_uint8(argv[2], &width)) {
		printf("Invalid width: %s\n", argv[2]);
		return EXIT_FAILURE;
	}
	if (!parse_uint8(argv[3], &height)) {
		printf("Invalid height: %s\n", argv[3]);
		return EXIT_FAILURE;
	}
	asciigolgen_result_t result = asciigolgen(argv[1], &width, &height);
	if (result != ASCIIGOLGEN_OK && result != ASCIIGOLGEN_DONE)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

