/**
 * @file main.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include "asciigol.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* USAGE =
	"Usage: <parameter>=<value>\n"
	"Parameter   | Value\n"
	"------------+---------------------\n"
	"--width     | Non-negative integer\n"
	"--height    | Non-negative integer\n"
	"--delay     | Non-negative integer\n"
	"--live-char | ASCII character\n"
	"--dead-char | ASCII character\n"
	"--file      | Name of file on system\n"
	"--wrap      | Literal true or false\n";

static bool parse_uint8(char* const arg, uint8_t* value) {
	if (!arg || !value)
		return false;
	int64_t temp_value;
	if (!sscanf(arg, "%ld", &temp_value))
		return false;
	if (temp_value < 0 || temp_value > UINT8_MAX)
		return false;
	*value = (uint8_t)temp_value;
	return true;
}

static bool parse_uint16(char* const arg, uint16_t* value) {
	if (!arg || !value)
		return false;
	int64_t temp_value;
	if (!sscanf(arg, "%ld", &temp_value))
		return false;
	if (temp_value < 0 || temp_value > UINT16_MAX)
		return false;
	*value = (uint16_t)temp_value;
	return true;
}

static bool parse_char(char* const arg, char* character) {
	if (!arg || !character)
		return false;
	if (!sscanf(arg, "%c", character))
		return false;
	return true;
}

static bool parse_string(char* const arg, char** string) {
	if (!arg || !string)
		return false;
	(*string) = arg;
	return true;
}

static bool parse_bool(char* const arg, bool* boolean) {
	if (!arg || !boolean)
		return false;
	char* temp_string;
	if (!parse_string(arg, &temp_string))
		return false;
	if (!strcmp(temp_string, "true"))
		(*boolean) = true;
	else if (!strcmp(temp_string, "false"))
		(*boolean) = false;
	else
		return false;
	return true;
}

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

static bool parse_args(asciigol_args_t* const args, const int argc, char** const argv) {
	for (int i = 1; i < argc; i++) {
		char* const arg = argv[i];
		if (!parse_arg(args, arg)) {
			printf("Failed to parse: %s\n%s", arg, USAGE);
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	asciigol_args_t args = { 0 };
	if (!parse_args(&args, argc, argv))
		return EXIT_FAILURE;
	asciigol_result_t result = asciigol(args);
	printf("asciigol terminated with result: %d\n", result);
	if (result != ASCIIGOL_OK || result != ASCIIGOL_CONVERGED)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

