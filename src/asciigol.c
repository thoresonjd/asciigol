/**
 * @file asciigol.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include <asciigol.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef uint8_t cell_t;

static const uint8_t DEFAULT_WIDTH = 100;
static const uint8_t DEFAULT_HEIGHT = 40;
static const uint16_t DEFAULT_DELAY_MILLIS = 50;
static const uint16_t MICROS_PER_MILLI = 1000;
static const char DEFAULT_LIVE_CHAR = '#';
static const char DEFAULT_DEAD_CHAR = ' ';

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static void wait(const uint16_t* const delay) {
	usleep((*delay ? *delay : DEFAULT_DELAY_MILLIS) * MICROS_PER_MILLI);
}

static asciigol_result_t init_cells_from_file(
	cell_t** cells,
	uint8_t* const width,
	uint8_t* const height,
	char* const filename
) {
	char character;
	int64_t temp_width, temp_height;
	uint16_t size, index = 0;
	uint8_t row = 0, col = 0;
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
	if (sscanf(line, "%ld,%ld", &temp_width, &temp_height) < 2) {
		result = ASCIIGOL_BAD_DIMENSION;
		goto EXIT;
	}
	if (temp_width <= 0 || temp_width > UINT8_MAX || temp_height <= 0 || temp_height > UINT8_MAX) {
		result = ASCIIGOL_BAD_DIMENSION;
		goto EXIT;
	}
	*width = (uint8_t)temp_width;
	*height = (uint8_t)temp_height;

	/****************************
	 * read initial cell states *
	 ****************************/

	size = (uint16_t)(*width * *height);
	*cells = (cell_t*)malloc(size);
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

		// convert '0' or '1' to cell/integer representation
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

static asciigol_result_t init_cells_at_random(
	cell_t** cells,
	uint8_t* const width,
	uint8_t* const height
) {
	if (*width > UINT8_MAX || *height > UINT8_MAX)
		return ASCIIGOL_BAD_DIMENSION;
	*width = *width ? *width : DEFAULT_WIDTH;
	*height = *height ? *height : DEFAULT_HEIGHT;
	uint16_t size = *width * *height;
	*cells = (cell_t*)malloc(size);
	if (!cells)
		return ASCIIGOL_BAD_DIMENSION;
	srand(time(NULL));
	for (uint16_t i = 0; i < size; i++)
		(*cells)[i] = (cell_t)(rand() % 2);
	return ASCIIGOL_OK;
}

static asciigol_result_t init_cells(
	cell_t** cells,
	uint8_t* const width,
	uint8_t* const height,
	char* const filename
) {
	if (filename)
		return init_cells_from_file(cells, width, height, filename);
	return init_cells_at_random(cells, width, height);
}

static uint8_t count_live_neighbors(
	cell_t** const cells,
	const uint8_t* const row,
	const uint8_t* const col,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
) {
	// get neighbor range; will go out of grid range when wrap is enabled
	int8_t row_begin = *row || *wrap ? *row - 1 : *row;
	int8_t col_begin = *col || *wrap ? *col - 1 : *col;
	int8_t row_end = *row < *height - 1 || *wrap ? *row + 1 : *row;
	int8_t col_end = *col < *width - 1 || *wrap ? *col + 1 : *col;
	uint8_t num_live_neighbors = 0;
	for (int8_t r = row_begin; r <= row_end; r++) {

		// account for wrap around
		uint8_t neighbor_row;
		if (r == -1)
			neighbor_row = *height - 1;
		else if (r == *height)
			neighbor_row = 0;
		else
			neighbor_row = (uint8_t)r;

		for (int8_t c = col_begin; c <= col_end; c++) {

			// account for wrap around
			uint8_t neighbor_col;
			if (c == -1)
				neighbor_col = *width - 1;
			else if (c == *width)
				neighbor_col = 0;
			else
				neighbor_col = (uint8_t)c;

			// count neighbor if live
			cell_t neighbor = (*cells)[*width * neighbor_row + neighbor_col];
			if (neighbor && (neighbor_row != *row || neighbor_col != *col))
				num_live_neighbors++;
		}
	}
	return num_live_neighbors;
}

static cell_t compute_game_of_life(
	const cell_t* const cell,
	const uint8_t* const num_live_neighbors
) {
	// Game of Life rules
	if (*cell && *num_live_neighbors < 2)
		return 0;
	if (*cell && (*num_live_neighbors == 2 || *num_live_neighbors == 3))
		return 1;
	if (*cell && *num_live_neighbors > 3)
		return 0;
	if (!*cell && *num_live_neighbors == 3)
		return 1;
	return 0;
}

static cell_t compute_cell(
	cell_t** const cells,
	const uint8_t* const row,
	const uint8_t* const col,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
) {
	cell_t cell = (*cells)[*width * *row + *col];
	uint8_t num_live_neighbors = count_live_neighbors(cells, row, col, width, height, wrap);
	return (cell_t)compute_game_of_life(&cell, &num_live_neighbors);
}

static asciigol_result_t compute_cells(
	cell_t** cells,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
) {
	uint16_t size = *width * *height;
	cell_t* new_cells = (cell_t*)malloc(size);
	bool converged = true;
	for (uint16_t i = 0; i < size; i++) {
		uint8_t row = (uint8_t)(i / *width);
		uint8_t col = (uint8_t)(i % *width);
		cell_t cell = (*cells)[i];
		cell_t new_cell = compute_cell(cells, &row, &col, width, height, wrap);
		if (cell != new_cell)
			converged = false;
		new_cells[i] = new_cell;
	}
	free(*cells);
	*cells = new_cells;
	return converged ? ASCIIGOL_CONVERGED : ASCIIGOL_OK;
}

static void render_cells(
	cell_t** const cells,
	const uint8_t* const width,
	const uint8_t* const height,
	const char* const live_char,
	const char* const dead_char
) {
	uint16_t size = *width * *height;
	for (uint16_t i = 0; i < size; i++) {
		const char character = (*cells)[i] ?
				(*live_char ? *live_char : DEFAULT_LIVE_CHAR) :
				(*dead_char ? *dead_char : DEFAULT_DEAD_CHAR);
		putchar(character);
		if (i % *width == *width - 1)
			putchar('\n');
	}
}

asciigol_result_t asciigol(asciigol_args_t args) {
	cell_t* cells = NULL;
	asciigol_result_t result = init_cells(&cells, &args.width, &args.height, args.filename);
	if (result != ASCIIGOL_OK)
		return result;
	clear_screen();
	while (result != ASCIIGOL_CONVERGED) {
		reset_cursor();
		render_cells(&cells, &args.width, &args.height, &args.live_char, &args.dead_char);
		result = compute_cells(&cells, &args.width, &args.height, &args.wrap);
		wait(&args.delay);
	}
	if (cells) {
		free(cells);
		cells = NULL;
	}
	return result;
}

