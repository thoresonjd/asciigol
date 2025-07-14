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

static const char* USAGE =
	"Usage: asciigolgen [arguments]\n"
	"Parameters:\n"
	"\t--width=<uint8>  width of asciigol grid to configure\n"
	"\t--height=<uint8> height of asciigol grid to configure\n"
	"\t--file=<string>  name of configuration file to generate";

static bool parse_arg(asciigolgen_args_t* const args, char* const arg) {
	const char WIDTH_PARAM[] = "--width=";
	const char HEIGHT_PARAM[] = "--height=";
	const char FILE_PARAM[] = "--file=";
	const size_t WIDTH_PARAM_SIZE = sizeof(WIDTH_PARAM) - 1;
	const size_t HEIGHT_PARAM_SIZE = sizeof(HEIGHT_PARAM) - 1;
	const size_t FILE_PARAM_SIZE = sizeof(FILE_PARAM) - 1;
	if (!strncmp(WIDTH_PARAM, arg, WIDTH_PARAM_SIZE) && !args->width)
		return parse_uint8(arg + WIDTH_PARAM_SIZE, &args->width);
	if (!strncmp(HEIGHT_PARAM, arg, HEIGHT_PARAM_SIZE) && !args->height)
		return parse_uint8(arg + HEIGHT_PARAM_SIZE, &args->height);
	if (!strncmp(FILE_PARAM, arg, FILE_PARAM_SIZE) && !args->filename)
		return parse_string(arg + FILE_PARAM_SIZE, &args->filename);
	return true;
}

static bool parse_args(
	asciigolgen_args_t* const args,
	const int* const argc,
	char** const argv
) {
	if (*argc != 4) {
		printf("%s\n", USAGE);
		return false;
	}
	for (int i = 1; i < *argc; i++) {
		char* const arg = argv[i];
		if (!parse_arg(args, arg)) {
			printf("Failed to parse: %s\n%s\n", arg, USAGE);
			return false;
		}
	}
	return true;
}

bool is_asciigolgen_success(const asciigolgen_result_t* const result) {
	return *result == ASCIIGOLGEN_OK || *result == ASCIIGOLGEN_DONE;
}

int main(int argc, char** argv) {
	asciigolgen_args_t args = { 0 };
	if (!parse_args(&args, &argc, argv))
		return EXIT_FAILURE;
	asciigolgen_result_t result = asciigolgen(args);
	return is_asciigolgen_success(&result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

