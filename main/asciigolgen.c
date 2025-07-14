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

static bool parse_arg(asciigolgen_args_t* const args, char* arg) {
	if (!args->width && skip_prefix(&arg, "--width="))
		return parse_uint8(arg, &args->width);
	if (!args->height && skip_prefix(&arg, "--height="))
		return parse_uint8(arg, &args->height);
	if (!args->filename && skip_prefix(&arg, "--file="))
		return parse_string(arg, &args->filename);
	return false;
}

static bool parse_args(
	asciigolgen_args_t* const args,
	const int* const argc,
	char** const argv
) {
	if (*argc != 4) {
		printf("Invalid number of arguments\n%s\n", USAGE);
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

