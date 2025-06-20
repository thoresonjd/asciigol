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

static bool parse_args(asciigol_args_t* const args, const int argc, char** const argv) {
	const char WIDTH_PARAM[] = "--width=";
	const char HEIGHT_PARAM[] = "--height=";
	const char DELAY_PARAM[] = "--delay=";
	const char LIVE_CHAR_PARAM[] = "--live-char=";
	const char DEAD_CHAR_PARAM[] = "--dead-char=";
	const char FILE_PARAM[] = "--file=";
	const char WRAP_PARAM[] = "--wrap";
	const size_t WIDTH_PARAM_SIZE = sizeof(WIDTH_PARAM) - 1;
	const size_t HEIGHT_PARAM_SIZE = sizeof(HEIGHT_PARAM) - 1;
	const size_t DELAY_PARAM_SIZE = sizeof(DELAY_PARAM) - 1;
	const size_t LIVE_CHAR_PARAM_SIZE = sizeof(LIVE_CHAR_PARAM) - 1;
	const size_t DEAD_CHAR_PARAM_SIZE = sizeof(DEAD_CHAR_PARAM) - 1;
	const size_t FILE_PARAM_SIZE = sizeof(FILE_PARAM) - 1;
	for (int i = 1; i < argc; i++) {
		char* const arg = argv[i];
		const size_t arg_size = strlen(arg);
		int res = -1;
		if (!strncmp(WIDTH_PARAM, arg, WIDTH_PARAM_SIZE) && !args->height) {
			// TODO: parsing uint directly is problematic: negative input will
			//       wrap around to the upper end of the uint range
			res = sscanf(arg + WIDTH_PARAM_SIZE, "%u", &args->width);
		} else if (!strncmp(HEIGHT_PARAM, arg, HEIGHT_PARAM_SIZE) && !args->height) {
			res = sscanf(arg + HEIGHT_PARAM_SIZE, "%u", &args->height);
		} else if (!strncmp(DELAY_PARAM, arg, DELAY_PARAM_SIZE) && !args->delay) {
			res = sscanf(arg + DELAY_PARAM_SIZE, "%u", &args->delay);
		} else if (!strncmp(LIVE_CHAR_PARAM, arg, LIVE_CHAR_PARAM_SIZE) && !args->live_char) {
			res = sscanf(arg + LIVE_CHAR_PARAM_SIZE, "%c", &args->live_char);
		} else if (!strncmp(DEAD_CHAR_PARAM, arg, DEAD_CHAR_PARAM_SIZE) && !args->dead_char) {
			res = sscanf(arg + DEAD_CHAR_PARAM_SIZE, "%c", &args->dead_char);
		} else if (!strncmp(FILE_PARAM, arg, FILE_PARAM_SIZE) && arg_size > FILE_PARAM_SIZE && !args->filename) {
			args->filename = arg + FILE_PARAM_SIZE;
		} else if (!strcmp(WRAP_PARAM, arg) && !args->wrap) {
			args->wrap = true;
		}
		if (!res)
			return false;
	}
	return true;
}

int main(int argc, char** argv) {
	asciigol_args_t args;
	if (parse_args(&args, argc, argv))
		printf("asciigol return code: %d\n", asciigol(args));
	return 0;
}

