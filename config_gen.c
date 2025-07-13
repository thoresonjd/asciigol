/**
 * @file config_gen.c
 * @brief Configuration file generator for ASCII Conway's Game of Life (asciigol).
 * @author Justin Thoreson
 * @date 2025
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

typedef enum {
	CONFIG_GEN_OK,
	CONFIG_GEN_DONE,
	CONFIG_GEN_INVAL,
	CONFIG_GEN_FAIL
} config_gen_result_t;

typedef char cell_t;
typedef char direction_t;

static const cell_t LIVE_CELL = '1';
static const cell_t DEAD_CELL = '0';
static const direction_t UP = 'A';
static const direction_t DOWN = 'B';
static const direction_t RIGHT = 'C';
static const direction_t LEFT = 'D';
static const char QUIT = 'q';
static const char* RUN_USAGE = "Usage: ./config_gen <filename> <width> <height>";
static const char* MODIFY_USAGE = "Move: Up, Down, Left, Right\nModify: 0, 1\nQuit: q";

static struct termios orig_termios;

static bool parse_uint8(char* const arg, uint8_t* const value) {
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

static config_gen_result_t init_terminal() {
	struct termios current_termios;
	if (tcgetattr(STDIN_FILENO, &orig_termios))
		return CONFIG_GEN_FAIL;
	current_termios = orig_termios;
	current_termios.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &current_termios))
		return CONFIG_GEN_FAIL;
	if (fflush(stdout))
		return CONFIG_GEN_FAIL;
	return CONFIG_GEN_OK;
}

static config_gen_result_t reset_terminal() {
	if (tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios))
		return CONFIG_GEN_FAIL;
	return CONFIG_GEN_OK;
}

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static config_gen_result_t init_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* const height
) {
	if (!width || !height)
		return CONFIG_GEN_INVAL;
	const uint16_t size = *width * *height;
	*state = (cell_t*)malloc(size);
	if (!*state)
		return CONFIG_GEN_FAIL;
	for (uint16_t i = 0; i < size; i++)
		(*state)[i] = DEAD_CELL;
	return CONFIG_GEN_OK;
}

static config_gen_result_t print_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height,
	const uint16_t* const highlight_idx
) {
	if (!state || !width || !height || !highlight_idx)
		return CONFIG_GEN_INVAL;
	const uint16_t size = *width * *height;
	for (uint16_t i = 0; i < size; i++) {
		if (i == *highlight_idx)
			printf("\x1b[32m%c\x1b[0m", (*state)[i]);
		else 
			putchar((*state)[i]);
		if (i % *width == *width - 1)
			putchar('\n');
	}
	printf("\n%s\n", MODIFY_USAGE);
	return CONFIG_GEN_OK;
}

static config_gen_result_t process_input(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height,
	uint16_t* const highlight_idx
) {
	if (!state || !width || !height || !highlight_idx)
		return CONFIG_GEN_INVAL;
	char c;
	if (read(STDIN_FILENO, &c, 1) <= 0)
		return CONFIG_GEN_FAIL;
	if (c == QUIT)
		return CONFIG_GEN_DONE;
	else if (c == LIVE_CELL || c == DEAD_CELL)
		(*state)[*highlight_idx] = c;
	else if (c == '\x1b') { // ANSI escape code
		getchar(); // skip [
		char value = getchar();
		const uint16_t size = *width * *height;
		if (value == UP && *highlight_idx >= *width)
			(*highlight_idx) -= *width;
		else if (value == DOWN && *highlight_idx < size)
			(*highlight_idx) += *width;
		else if (value == RIGHT && *highlight_idx < size - 1)
			(*highlight_idx)++;
		else if (value == LEFT && *highlight_idx > 0)
			(*highlight_idx)--;
	}
	return CONFIG_GEN_OK;
}

static config_gen_result_t modify_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height
) {
	if (!state || !width || !height)
		return CONFIG_GEN_INVAL;
	uint16_t highlight_idx = 0;
	config_gen_result_t result = CONFIG_GEN_OK;
	clear_screen();
	do {
		reset_cursor();
		result = print_state(state, width, height, &highlight_idx);
		if (result != CONFIG_GEN_OK)
			return result;
		result = process_input(state, width, height, &highlight_idx);
	} while (result == CONFIG_GEN_OK);
	return result;
}


static config_gen_result_t write_config(
	char* const filename,
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* const height
) {
	if (!filename || !state || !width || !height)
		return CONFIG_GEN_INVAL;
	FILE* file = fopen(filename, "w");
	if (!file)
		return CONFIG_GEN_FAIL;
	const char HEADER[] = "asciigol\n";
	const size_t HEADER_SIZE = sizeof(HEADER) - 1;
	fwrite(HEADER, HEADER_SIZE, 1, file);
	fprintf(file, "%u,%u\n", *width, *height);
	const uint16_t size = *width * *height;
	for (uint16_t i = 0; i < size; i++) {
		fputc((*state)[i], file);
		if (i % *width == *width - 1)
			fputc('\n', file);
	}
	fclose(file);
	return CONFIG_GEN_OK;
}

config_gen_result_t config_gen(
	char* const filename,
	const uint8_t* const width,
	const uint8_t* const height
) {
	if (!filename || !width || !height)
		return CONFIG_GEN_INVAL;
	cell_t* state = NULL;
	config_gen_result_t result = CONFIG_GEN_OK;
	result = init_state(&state, width, height);
	if (result != CONFIG_GEN_OK) {
		printf("Failed to initialize state - result: %d\n", result);
		goto EXIT;
	}
	result = init_terminal();
	if (result != CONFIG_GEN_OK) {
		printf("Failed to initialize terminal - result: %d\n", result);
		goto EXIT;
	}
	result = modify_state(&state, width, height);
	if (result != CONFIG_GEN_OK && result != CONFIG_GEN_DONE) {
		printf("Failed to mofify state - result: %d\n", result);
		goto EXIT;
	}
	result = write_config(filename, &state, width, height);
	if (result != CONFIG_GEN_OK)
		printf("Failed to write config to file %s - result: %d\n", filename, result);
EXIT:
	result = reset_terminal();
	if (result != CONFIG_GEN_OK)
		printf("Failed to reset terminal - result: %d\n", result);
	if (state)
		free(state);
	return result;
}

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("%s\n", RUN_USAGE);
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
	config_gen_result_t result = config_gen(argv[1], &width, &height);
	if (result != CONFIG_GEN_OK && result != CONFIG_GEN_DONE)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
