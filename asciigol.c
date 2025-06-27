/**
 * @file asciigol.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include "asciigol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static const unsigned int DEFAULT_WIDTH = 100;
static const unsigned int DEFAULT_HEIGHT = 40;
static const unsigned int MAX_WIDTH = 250;
static const unsigned int MAX_HEIGHT = 100;
static const unsigned int DEFAULT_DELAY_MILLIS = 50;
static const unsigned int MICROS_PER_MILLI = 1000;
static const char DEFAULT_LIVE_CHAR = '#';
static const char DEFAULT_DEAD_CHAR = ' ';

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static void wait(const unsigned int delay) {
	usleep((delay ? delay : DEFAULT_DELAY_MILLIS) * MICROS_PER_MILLI);
}

static asciigol_result_t init_cells_from_file(char** cells, unsigned int* const width, unsigned int* const height, char* const filename) {
	char character;
	int temp_width, temp_height;
	unsigned int size;
	unsigned int index = 0, row = 0, col = 0;
	char* line = NULL;
	size_t line_len = 0;
	asciigol_result_t result = ASCIIGOL_OK;
	FILE* file = fopen(filename, "r");

	/****************************************
	 * read constant first line: "asciigol" *
	 ****************************************/

	if (!file) {
		result = ASCIIGOL_BAD_FILE;
		goto EXIT;
	}
	if (getline(&line, &line_len, file) <= 0) {
		result = ASCIIGOL_BAD_HEADER;
		goto EXIT;
	}
	if (strcmp(line, "asciigol\n")) {
		result = ASCIIGOL_BAD_HEADER;
		goto EXIT;
	}

	/*************************
	 * read width and height *
	 *************************/

	free(line);
	line = NULL;
	if (getline(&line, &line_len, file) <= 0) {
		result = ASCIIGOL_BAD_DIMENSION;
		goto EXIT;
	}
	if (sscanf(line, "%d,%d", &temp_width, &temp_height) < 2) {
		result = ASCIIGOL_BAD_DIMENSION;
		goto EXIT;
	}
	if (temp_width <= 0 || temp_width > MAX_WIDTH || temp_height <= 0 || temp_height > MAX_HEIGHT) {
		result = ASCIIGOL_BAD_DIMENSION;
		goto EXIT;
	}
	*width = (unsigned int)temp_width;
	*height = (unsigned int)temp_height;

	/****************************
	 * read initial cell states *
	 ****************************/

	size = *width * *height;
	*cells = (char*)malloc(size);
	if (!cells) {
		result = ASCIIGOL_BAD_DIMENSION;
		goto EXIT;
	}

	// iterate through remaining characters of file
	while((character = getc(file)) != EOF) {

		// error if number of rows greater than specified height
		if (row >= *height) {
			result = ASCIIGOL_BAD_DIMENSION;
			goto EXIT;
		}

		// reached end of row
		if (character == '\n') {
			// error if number of columns less than specified width
			if (col < *width) {
				result = ASCIIGOL_BAD_DIMENSION;
				goto EXIT;
			}
			row++;
			col = 0;
			continue;
		}

		// error if cell is not '0' or '1'
		if (character != '0' && character != '1') {
			result = ASCIIGOL_BAD_CELL;
			goto EXIT;
		}

		// error if number of columns greater than specified width
		if (++col > *width) {
			result = ASCIIGOL_BAD_DIMENSION;
			goto EXIT;
		}

		// convert '0' or '1' to integer representation
		(*cells)[index++] = character - '0';
	}

	// error if number of rows less than specified height
	if (row < *height)
		result = ASCIIGOL_BAD_DIMENSION;

	/***********
	 * cleanup *
	 ***********/

EXIT:
	if (line) {
		free(line);
		line = NULL;
	}
	if (*cells && result != ASCIIGOL_OK) {
		free(*cells);
		*cells = NULL;
	}
	if (file) {
		fclose(file);
		file = NULL;
	}
	return result;
}

static asciigol_result_t init_cells_at_random(char** cells, unsigned int* const width, unsigned int* const height) {
	if (*width > MAX_WIDTH || *height > MAX_HEIGHT)
		return ASCIIGOL_BAD_DIMENSION;
	*width = *width ? *width : DEFAULT_WIDTH;
	*height = *height ? *height : DEFAULT_HEIGHT;
	int size = *width * *height;
	*cells = (char*)malloc(size);
	if (!cells)
		return ASCIIGOL_BAD_DIMENSION;
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		(*cells)[i] = rand() % 2;
	return ASCIIGOL_OK;
}

static asciigol_result_t init_cells(char** cells, unsigned int* const width, unsigned int* const height, char* const filename) {
	if (filename)
		return init_cells_from_file(cells, width, height, filename);
	return init_cells_at_random(cells, width, height);
}

static char compute_cell(char** cells, int row, int col, int width, int height, bool wrap) {
	char cell = (*cells)[width * row + col];
	// get neighbor range; will go out of grid range when wrap is enabled
	int row_begin = row || wrap ? row - 1 : row;
	int col_begin = col || wrap ? col - 1 : col;
	int row_end = row < height - 1 || wrap ? row + 1 : row;
	int col_end = col < width - 1 || wrap ? col + 1 : col;
	int num_live_neighbors = 0;
	for (int r = row_begin; r <= row_end; r++) {

		// account for wrap around
		int neighbor_row = r;
		if (neighbor_row == -1)
			neighbor_row = height - 1;
		else if (neighbor_row == height)
			neighbor_row = 0;

		for (int c = col_begin; c <= col_end; c++) {

			// account for wrap around
			int neighbor_col = c;
			if (neighbor_col == -1)
				neighbor_col = width - 1;
			else if (neighbor_col == width)
				neighbor_col = 0;

			// count neighbor if live
			int neighbor = (*cells)[width * neighbor_row + neighbor_col];
			if (neighbor && (neighbor_row != row || neighbor_col != col))
				num_live_neighbors++;
		}
	}
	// GOL rules
	if (cell && num_live_neighbors < 2)
		return 0;
	if (cell && (num_live_neighbors == 2 || num_live_neighbors == 3))
		return 1;
	if (cell && num_live_neighbors > 3)
		return 0;
	if (!cell && num_live_neighbors == 3)
		return 1;
	return 0;
}

static asciigol_result_t compute_cells(char** cells, int width, int height, bool wrap) {
	int size = width * height;
	char* new_cells = (char*)malloc(size);
	bool converged = true;
	for (int i = 0; i < size; i++) {
		int row = i / width;
		int col = i % width;
		char cell = (*cells)[i];
		char new_cell = compute_cell(cells, row, col, width, height, wrap);
		if (cell != new_cell)
			converged = false;
		new_cells[i] = new_cell;
	}
	free(*cells);
	*cells = new_cells;
	return converged ? ASCIIGOL_CONVERGED : ASCIIGOL_OK;
}

static void render_cells(char** cells, const int width, const int height, const char live_char, const char dead_char) {
	int size = width * height;
	for (int i = 0; i < size; i++) {
		const char character = (*cells)[i] ?
				(live_char ? live_char : DEFAULT_LIVE_CHAR) :
				(dead_char ? dead_char : DEFAULT_DEAD_CHAR);
		putchar(character);
		if (i % width == width - 1)
			putchar('\n');
	}
}

asciigol_result_t asciigol(asciigol_args_t args) {
	char* cells = NULL;
	asciigol_result_t result = init_cells(&cells, &args.width, &args.height, args.filename);
	if (result != ASCIIGOL_OK)
		return result;
	clear_screen();
	while (result != ASCIIGOL_CONVERGED) {
		reset_cursor();
		render_cells(&cells, args.width, args.height, args.live_char, args.dead_char);
		result = compute_cells(&cells, args.width, args.height, args.wrap);
		wait(args.delay);
	}
	return result;
}

