/**
 * @file asciigolgen.c
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

/**
 * @brief Usage information explaining how to run the program.
 */
static const char* USAGE =
	"Usage: asciigolgen [arguments]\n"
	"Parameters:\n"
	"\t--file=<string>  name of configuration file to generate\n"
	"\t--width=<uint8>  width of asciigol grid to configure\n"
	"\t--height=<uint8> height of asciigol grid to configure\n"
	"\t--cell=0|1       the cell state to initialize with";

/**
 * @brief Parse a provided command-line argument.
 * @param[in,out] args The parsed arguments.
 * @param[in] arg The argument to parse.
 * @return True if the argument was parsed successfully, false otherwise.
 */
static bool parse_arg(asciigolgen_args_t* const args, char* arg);

/**
 * @brief Parse provided command-line arguments.
 * @param[in,out] args The parsed arguments.
 * @param[in] argc The number of arguments to parse.
 * @param[in] argv The list of arguments to parse.
 * @return True if the arguments were parsed successfully, false otherwise.
 */
static bool parse_args(
	asciigolgen_args_t* const args,
	const int* const argc,
	char** const argv
);

/**
 * @brief Print the result of the asciigolgen program as text.
 * @param[in] result The asciigolgen result.
 */
static void print_asciigolgen_result(const asciigolgen_result_t* const result);

/**
 * @brief Determine if the asciigolgen program ran successfully.
 * @param[in] result The asciigolgen result.
 * @return True if asciigolgen ran successfully, false otherwise.
 */
static bool is_asciigolgen_success(const asciigolgen_result_t* const result);

int main(int argc, char** argv) {
	asciigolgen_args_t args = { 0 };
	if (!parse_args(&args, &argc, argv))
		return EXIT_FAILURE;
	asciigolgen_result_t result = asciigolgen(args);
	print_asciigolgen_result(&result);
	return is_asciigolgen_success(&result) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static bool parse_arg(asciigolgen_args_t* const args, char* arg) {
	if (!args->filename && skip_prefix(&arg, "--file=")) {
		args->filename = arg;
		return true;
	}
	if (!args->width && skip_prefix(&arg, "--width="))
		return parse_uint8(arg, &args->width);
	if (!args->height && skip_prefix(&arg, "--height="))
		return parse_uint8(arg, &args->height);
	if (!args->cell &&
	    skip_prefix(&arg, "--cell=") &&
	    parse_char(arg, &args->cell))
		return args->cell == '0' || args->cell == '1';
	return false;
}

static bool parse_args(
	asciigolgen_args_t* const args,
	const int* const argc,
	char** const argv
) {
	if (*argc != 5) {
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

static void print_asciigolgen_result(const asciigolgen_result_t* const result) {
	printf("Result: ");
	switch (*result) {
		case ASCIIGOLGEN_OK:
			printf("ASCIIGOLGEN_OK (%d)\n", ASCIIGOLGEN_FAIL);
			break;
		case ASCIIGOLGEN_DONE:
			printf("ASCIIGOLGEN_DONE (%d)\n", ASCIIGOLGEN_DONE);
			break;
		case ASCIIGOLGEN_INVAL:
			printf("ASCIIGOLGEN_INVAL (%d)\n", ASCIIGOLGEN_INVAL);
			break;
		case ASCIIGOLGEN_FAIL:
			printf("ASCIIGOLGEN_FAIL (%d)\n", ASCIIGOLGEN_FAIL);
			break;
		default:
			printf("result not recognized\n");
	}
}

static bool is_asciigolgen_success(const asciigolgen_result_t* const result) {
	return *result == ASCIIGOLGEN_OK || *result == ASCIIGOLGEN_DONE;
}

