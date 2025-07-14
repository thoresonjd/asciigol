/**
 * @file asciigol_main.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include <asciigol.h>
#include <parsing.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* USAGE =
	"Usage: asciigol [arguments]\n"
	"Parameters:\n"
	"\t--width=<uint8>    width of grid\n"
	"\t--height=<uint8>   height of grid\n"
	"\t--delay=<uint16>   delay between frames in milliseconds\n"
	"\t--live-char=<char> character representing a live cell\n"
	"\t--dead-char=<char> character representing a dead cell\n"
	"\t--file=<string>    custom configuration file\n"
	"\t--wrap=true|false  reaching row/column limit will\n"
	"\t                   wrap around to the other end\n";

static bool parse_arg(asciigol_args_t* const args, char* const arg) {
	const char WIDTH_PARAM[] = "--width=";
	const char HEIGHT_PARAM[] = "--height=";
	const char DELAY_PARAM[] = "--delay=";
	const char LIVE_CHAR_PARAM[] = "--live-char=";
	const char DEAD_CHAR_PARAM[] = "--dead-char=";
	const char FILE_PARAM[] = "--file=";
	const char WRAP_PARAM[] = "--wrap=";
	const size_t WIDTH_PARAM_SIZE = sizeof(WIDTH_PARAM) - 1;
	const size_t HEIGHT_PARAM_SIZE = sizeof(HEIGHT_PARAM) - 1;
	const size_t DELAY_PARAM_SIZE = sizeof(DELAY_PARAM) - 1;
	const size_t LIVE_CHAR_PARAM_SIZE = sizeof(LIVE_CHAR_PARAM) - 1;
	const size_t DEAD_CHAR_PARAM_SIZE = sizeof(DEAD_CHAR_PARAM) - 1;
	const size_t FILE_PARAM_SIZE = sizeof(FILE_PARAM) - 1;
	const size_t WRAP_PARAM_SIZE= sizeof(WRAP_PARAM) - 1;
	if (!strncmp(WIDTH_PARAM, arg, WIDTH_PARAM_SIZE) && !args->width)
		return parse_uint8(arg + WIDTH_PARAM_SIZE, &args->width);
	if (!strncmp(HEIGHT_PARAM, arg, HEIGHT_PARAM_SIZE) && !args->height)
		return parse_uint8(arg + HEIGHT_PARAM_SIZE, &args->height);
	if (!strncmp(DELAY_PARAM, arg, DELAY_PARAM_SIZE) && !args->delay)
		return parse_uint16(arg + DELAY_PARAM_SIZE, &args->delay);
	if (!strncmp(LIVE_CHAR_PARAM, arg, LIVE_CHAR_PARAM_SIZE) && !args->live_char)
		return parse_char(arg + LIVE_CHAR_PARAM_SIZE, &args->live_char);
	if (!strncmp(DEAD_CHAR_PARAM, arg, DEAD_CHAR_PARAM_SIZE) && !args->dead_char)
		return parse_char(arg + DEAD_CHAR_PARAM_SIZE, &args->dead_char);
	if (!strncmp(FILE_PARAM, arg, FILE_PARAM_SIZE) && !args->filename)
		return parse_string(arg + FILE_PARAM_SIZE, &args->filename);
	if (!strncmp(WRAP_PARAM, arg, WRAP_PARAM_SIZE) && !args->wrap)
		return parse_bool(arg + WRAP_PARAM_SIZE, &args->wrap);
	return true;
}

static bool parse_args(
	asciigol_args_t* const args,
	const int* const argc,
	char** const argv
) {
	for (int i = 1; i < *argc; i++) {
		char* const arg = argv[i];
		if (!parse_arg(args, arg)) {
			printf("Failed to parse: %s\n%s", arg, USAGE);
			return false;
		}
	}
	return true;
}

static void print_asciigol_result(const asciigol_result_t* const result) {
	printf("Result: ");
	switch (*result) {
		case ASCIIGOL_OK:
			printf("ASCIIGOL_OK (%d)\n", ASCIIGOL_OK);
			break;
		case ASCIIGOL_CONVERGED:
			printf("ASCIIGOL_CONVERGED (%d)\n", ASCIIGOL_CONVERGED);
			break;
		case ASCIIGOL_BAD_FILE:
			printf("ASCIIGOL_BAD_FILE (%d)\n", ASCIIGOL_BAD_FILE);
			break;
		case ASCIIGOL_BAD_HEADER:
			printf("ASCIIGOL_BAD_HEADER (%d)\n", ASCIIGOL_BAD_HEADER);
			break;
		case ASCIIGOL_BAD_DIMENSION:
			printf("ASCIIGOL_BAD_DIMENSION (%d)\n", ASCIIGOL_BAD_DIMENSION);
			break;
		case ASCIIGOL_BAD_CELL:
			printf("ASCIIGOL_BAD_CELL (%d)\n", ASCIIGOL_BAD_CELL);
			break;
		default:
			printf("result not recognized\n");
	}
}

bool is_asciigol_success(const asciigol_result_t* const result) {
	return *result == ASCIIGOL_OK || *result == ASCIIGOL_CONVERGED;
}

int main(int argc, char** argv) {
	asciigol_args_t args = { 0 };
	if (!parse_args(&args, &argc, argv))
		return EXIT_FAILURE;
	asciigol_result_t result = asciigol(args);
	print_asciigol_result(&result);
	return is_asciigol_success(&result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

