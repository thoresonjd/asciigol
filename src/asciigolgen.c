/**
 * @file asciigolgen.c
 * @brief Configuration file generator for asciigol.
 * @author Justin Thoreson
 * @date 2025
 */

#include <asciigolgen.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

typedef char cell_t;
typedef char direction_t;

static const cell_t LIVE_CELL = '1';
static const cell_t DEAD_CELL = '0';
static const direction_t UP = 'A';
static const direction_t DOWN = 'B';
static const direction_t RIGHT = 'C';
static const direction_t LEFT = 'D';
static const char QUIT = 'q';
static const char* CONTROLS = "Move: Up, Down, Left, Right\nModify: 0, 1\nQuit: q";

static struct termios get_terminal() {
	struct termios terminal;
	tcgetattr(STDIN_FILENO, &terminal);
	return terminal;
}

static void set_terminal(const struct termios* terminal) {
	tcsetattr(STDIN_FILENO, TCSANOW, terminal);
	fflush(stdout);
}

static struct termios terminal_noncanon(struct termios terminal) {
	terminal.c_lflag &= ~(ICANON | ECHO);
	return terminal;
}

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static asciigolgen_result_t init_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* const height
) {
	if (!width || !height)
		return ASCIIGOLGEN_INVAL;
	const uint16_t size = *width * *height;
	*state = (cell_t*)malloc(size);
	if (!*state)
		return ASCIIGOLGEN_FAIL;
	for (uint16_t i = 0; i < size; i++)
		(*state)[i] = DEAD_CELL;
	return ASCIIGOLGEN_OK;
}

static asciigolgen_result_t print_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height,
	const uint16_t* const highlight_idx
) {
	if (!state || !width || !height || !highlight_idx)
		return ASCIIGOLGEN_INVAL;
	const uint16_t size = *width * *height;
	for (uint16_t i = 0; i < size; i++) {
		if (i == *highlight_idx)
			printf("\x1b[32m%c\x1b[0m", (*state)[i]);
		else 
			putchar((*state)[i]);
		if (i % *width == *width - 1)
			putchar('\n');
	}
	printf("\n%s\n", CONTROLS);
	return ASCIIGOLGEN_OK;
}

static asciigolgen_result_t process_input(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height,
	uint16_t* const highlight_idx
) {
	if (!state || !width || !height || !highlight_idx)
		return ASCIIGOLGEN_INVAL;
	char c;
	if (read(STDIN_FILENO, &c, 1) <= 0)
		return ASCIIGOLGEN_FAIL;
	if (c == QUIT)
		return ASCIIGOLGEN_DONE;
	else if (c == LIVE_CELL || c == DEAD_CELL)
		(*state)[*highlight_idx] = c;
	else if (c == '\x1b') { // ANSI escape code
		getchar(); // skip [
		char value = getchar();
		const uint16_t size = *width * *height;
		if (value == UP && *highlight_idx >= *width)
			(*highlight_idx) -= *width;
		else if (value == DOWN && *highlight_idx < size - *width)
			(*highlight_idx) += *width;
		else if (value == RIGHT && *highlight_idx < size - 1)
			(*highlight_idx)++;
		else if (value == LEFT && *highlight_idx > 0)
			(*highlight_idx)--;
	}
	return ASCIIGOLGEN_OK;
}

static asciigolgen_result_t modify_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height
) {
	if (!state || !width || !height)
		return ASCIIGOLGEN_INVAL;
	uint16_t highlight_idx = 0;
	asciigolgen_result_t result = ASCIIGOLGEN_OK;
	clear_screen();
	do {
		reset_cursor();
		result = print_state(state, width, height, &highlight_idx);
		if (result != ASCIIGOLGEN_OK)
			return result;
		result = process_input(state, width, height, &highlight_idx);
	} while (result == ASCIIGOLGEN_OK);
	return result;
}


static asciigolgen_result_t write_config(
	char* const filename,
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* const height
) {
	if (!filename || !state || !width || !height)
		return ASCIIGOLGEN_INVAL;
	FILE* file = fopen(filename, "w");
	if (!file)
		return ASCIIGOLGEN_FAIL;
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
	return ASCIIGOLGEN_OK;
}

asciigolgen_result_t asciigolgen(asciigolgen_args_t args) {
	cell_t* state = NULL;
	asciigolgen_result_t result = init_state(&state, &args.width, &args.height);
	if (result != ASCIIGOLGEN_OK)
		return result;
	struct termios orig_terminal = get_terminal();
	struct termios new_terminal = terminal_noncanon(orig_terminal);
	set_terminal(&new_terminal);
	result = modify_state(&state, &args.width, &args.height);
	if (result == ASCIIGOLGEN_OK || result == ASCIIGOLGEN_DONE)
		result = write_config(args.filename, &state, &args.width, &args.height);
	set_terminal(&orig_terminal);
	if (state) {
		free(state);
		state = NULL;
	}
	return result;
}

