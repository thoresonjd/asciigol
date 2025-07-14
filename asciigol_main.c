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

static bool parse_arg(asciigol_args_t* const args, char* arg) {
	if (!args->width && skip_prefix(&arg, "--width="))
		return parse_uint8(arg, &args->width);
	if (!args->height && skip_prefix(&arg, "--height="))
		return parse_uint8(arg, &args->height);
	if (!args->delay && skip_prefix(&arg, "--delay="))
		return parse_uint16(arg, &args->delay);
	if (!args->live_char && skip_prefix(&arg, "--live-char="))
		return parse_char(arg, &args->live_char);
	if (!args->dead_char && skip_prefix(&arg, "--dead-char="))
		return parse_char(arg, &args->dead_char);
	if (!args->filename && skip_prefix(&arg, "--file="))
		return parse_string(arg, &args->filename);
	if (!args->wrap && skip_prefix(&arg, "--wrap="))
		return parse_bool(arg, &args->wrap);
	return false;
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

