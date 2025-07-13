/**
 * @file config_gen.c
 * @brief Configuration file generator for ASCII Conway's Game of Life (asciigol).
 * @author Justin Thoreson
 * @date 2025
 */

// TODO: cleanup
 
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

typedef char cell_t;

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

static bool init_terminal() {
	struct termios current_termios;
	if (tcgetattr(STDIN_FILENO, &orig_termios))
		return false;
	current_termios = orig_termios;
	current_termios.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &current_termios))
		return false;
	return !fflush(stdout);
}

static bool reset_terminal() {
	return !tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static bool init_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* const height
) {
	if (!width || !height)
		return false;
	const uint16_t size = *width * *height;
	*state = (cell_t*)malloc(size * sizeof(cell_t));
	for (uint16_t i = 0; i < size; i++)
		(*state)[i] = '0';
	return *state;
}

static void print_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height,
	const uint16_t* const highlight_idx
) {
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
}

static bool process_input(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height,
	uint16_t* const highlight_idx
) {
	if (!state || !width || !height || !highlight_idx)
		return false;
	char c;
	if (read(STDIN_FILENO, &c, 1) <= 0)
		return false;
	if (c == 'q')
		return false;
	if (c == '0' || c == '1') // valid cell states
		(*state)[*highlight_idx] = c;
	else if (c == '\x1b') { // ANSI escape code
		getchar(); // skip [
		char value = getchar();
		const uint16_t size = *width * *height;
		switch (value) { 
			case 'A': // up arrow 
				if (*highlight_idx >= *width)
					(*highlight_idx) -= *width;
				break;
			case 'B': // down arrow
				if (*highlight_idx < size)
					(*highlight_idx) += *width;
				break;
			case 'C': // right arrow
				if (*highlight_idx < size - 1)
					(*highlight_idx)++;
				break;
			case 'D': // left arrow
				if (*highlight_idx > 0)
					(*highlight_idx)--;
				break;
		}		
	}
	return true;
}

static bool modify_state(
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* height
) {
	if (!state || !width || !height)
		return false;
	uint16_t highlight_idx = 0;
	clear_screen();
	do {
		reset_cursor();
		print_state(state, width, height, &highlight_idx);
	} while (process_input(state, width, height, &highlight_idx));
	return true;
}


static bool write_config(
	char* const filename,
	cell_t** const state,
	const uint8_t* const width,
	const uint8_t* const height
) {
	FILE* file = fopen(filename, "w");
	if (!file)
		return false;
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
	return true;
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
	cell_t* state = NULL;
	if (!init_state(&state, &width, &height)) {
		printf("Failed to initialize state\n");
		goto EXIT;
	}
	if (!init_terminal()) {
		printf("Failed to initialize terminal\n");
		goto EXIT;
	}
	(void)modify_state(&state, &width, &height);
	if (!write_config(argv[1], &state, &width, &height))
		printf("Failed to write config to file %s\n", argv[1]);
EXIT:
	if (!reset_terminal())
		printf("Failed to reset terminal\n");
	if (state)
		free(state);
	return EXIT_SUCCESS;
}
