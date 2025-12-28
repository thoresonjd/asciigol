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

/**
 * @brief The data type representing a Game of Life cell.
 */
typedef uint8_t cell_t;

/**
 * @brief The default width of the Game of Life grid.
 */
static const uint8_t DEFAULT_WIDTH = 100;

/**
 * @brief The default height of the Game of Life grid.
 */
static const uint8_t DEFAULT_HEIGHT = 40;

/**
 * @brief The default delay between frames in milliseconds.
 */
static const uint16_t DEFAULT_DELAY_MILLIS = 50;

/**
 * @brief The number of microseconds per millisecond.
 */
static const uint16_t MICROS_PER_MILLI = 1000;

/**
 * @brief The default character representing a live cell.
 */
static const char DEFAULT_LIVE_CHAR = '#';

/**
 * @brief The default character representing a dead cell.
 */
static const char DEFAULT_DEAD_CHAR = ' ';

/**
 * @brief ANSI control code for white background with black foreground.
 */
static const char* BG_WHITE_FG_BLACK = "\x1b[47;30m";

/**
 * @brief ANSI control code for black background with white foreground.
 */
static const char* BG_BLACK_FG_WHITE = "\x1b[40;37m";

/**
 * @brief ANSI control code to reset terminal attributes (colors) to default.
 */
static const char* BG_DEFAULT_FG_DEFAULT = "\x1b[0m";

/**
 * @brief Clear the contents of the terminal screen.
 */
static void clear_screen();

/**
 * @brief Reset the cursor to the top-left position.
 */
static void reset_cursor();

/**
 * @brief Pause execution for a provided number of milliseconds.
 * @param[in] delay The number of milliseconds to sleep for.
 */
static void wait(const uint16_t* const delay);

/**
 * @brief Deallocate a provided heap-allocated buffer.
 * @param[in] buffer The buffer to deallocate.
 */
static void free_buffer(cell_t** buffer);

/**
 * @brief Initialize the Game of Life cells from a provided file.
 * @param[out] cells The cells comprising the Game of Life grid.
 * @param[out] width The width of the Game of Life grid.
 * @param[out] height The height of the Game of Life grid.
 * @param[in] filename The name of the file to initialize the cells from.
 * @return The result of the initialization.
 */
static asciigol_result_t init_cells_from_file(
	cell_t** cells,
	uint8_t* const width,
	uint8_t* const height,
	char* const filename
);

/**
 * @brief Initialize the Game of Life cells at random.
 * @param[out] cells The cells comprising the Game of Life grid.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @return The result of the initialization.
 */
static asciigol_result_t init_cells_at_random(
	cell_t** cells,
	uint8_t* const width,
	uint8_t* const height
);

/**
 * @brief Initialize the back-buffer for the Game of Life cells.
 * @param[out] back_buffer The back-buffer for the Game of Life cells.
 * @param[in] size The size that should be allocated.
 * @return The result of the initialization.
 */
static asciigol_result_t init_back_buffer(
	cell_t** back_buffer,
	const uint16_t size
);

/**
 * @brief Initialize the Game of Life cells.
 * @param[out] cells The cells comprising the Game of Life grid.
 * @param[out] back_buffer The back-buffer for the Game of Life cells.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @return The result of the initialization.
 */
static asciigol_result_t init_cells(
	cell_t** cells,
	cell_t** back_buffer,
	uint8_t* const width,
	uint8_t* const height,
	char* const filename
);

/**
 * @brief Count the number of live neighbors surrounding a cell.
 * @param[in] cells The cells in the Game of Life grid.
 * @param[in] row The row of the cell to count the neighbors of.
 * @param[in] col The column of the cell to count the neighbors of.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] wrap Specify whether, if the cell is residing on an edge of the
 *                 grid, to count the neighbors along the opposite edge of said
 *                 cell, wrapping around as if both edges were connected.
 * @return The number of live neighbors surrounding the specified cell.
 */
static uint8_t count_live_neighbors(
	cell_t** const cells,
	const uint8_t* const row,
	const uint8_t* const col,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
);

/**
 * @brief Determine if a cell should live or die.
 * @param[in] cell A cell in the Game of Life grid.
 * @param[in] num_live_neighbors The number of living cells neighboring the
 *                               given cell.
 * @return The new value of the provided cell: live or dead.
 */
static cell_t compute_game_of_life(
	const cell_t* const cell,
	const uint8_t* const num_live_neighbors
);

/**
 * @brief Compute the new value of a cell.
 * @param[in] cells The cells comprising the Game of Life grid.
 * @param[in] row The row of the cell to recompute.
 * @param[in] col The column of the cell to recompute.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] wrap Specify whether, if the cell is residing on an edge of the
 *                 grid, to count the neighbors along the opposite edge of said
 *                 cell, wrapping around as if both edges were connected.
 * @return The new value of the specified cell.
 */
static cell_t compute_cell(
	cell_t** const cells,
	const uint8_t* const row,
	const uint8_t* const col,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
);

/**
 * @brief Compute the next iteration of cells.
 * @param[in] cells The cells comprising the Game of Life grid.
 * @param[out] new_cells The newly-computed cells of the Game of Life grid.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] wrap Specify whether, if the cell is residing on an edge of the
 *                 grid, to count the neighbors along the opposite edge of said
 *                 cell, wrapping around as if both edges were connected.
 * @return The result of computing the next Game of Life iteration.
 */
static asciigol_result_t compute_cells(
	cell_t** cells,
	cell_t** new_cells,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
);

/**
 * @brief Swap two buffers.
 * @param[in,out] buffer_a The first buffer.
 * @param[in,out] buffer_b The second buffer.
 */
static void swap_buffers(cell_t** buffer_a, cell_t** buffer_b);

/**
 * @brief Render the Game of Life cells.
 * @param[in] cells The cells comprising the Game of Life grid.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] live_char The character to render for a live cell.
 * @param[in] dead_char The character to render for a dead cell.
 * @param[in] background The background color type.
 */
static void render_cells(
	cell_t** const cells,
	const uint8_t* const width,
	const uint8_t* const height,
	const char* const live_char,
	const char* const dead_char,
	const asciigol_bg_t* const background
);

/**
 * @brief Deallocate the Game of Life buffers.
 * @param[in] cells The active buffer containing the Game of Life cells.
 * @param[in] back_buffer The back-buffer for the Game of Life cells.
 */
static void destroy_cells(cell_t** cells, cell_t** back_buffer);

asciigol_result_t asciigol(asciigol_args_t args) {
	cell_t* cells = NULL;
	cell_t* back_buffer = NULL;
	asciigol_result_t result = init_cells(&cells, &back_buffer, &args.width, &args.height, args.filename);
	if (result != ASCIIGOL_OK)
		return result;
	clear_screen();
	while (result != ASCIIGOL_CONVERGED) {
		reset_cursor();
		render_cells(&cells, &args.width, &args.height, &args.live_char, &args.dead_char, &args.background);
		result = compute_cells(&cells, &back_buffer, &args.width, &args.height, &args.wrap);
		swap_buffers(&cells, &back_buffer);
		wait(&args.delay);
	}
	destroy_cells(&cells, &back_buffer);
	return result;
}

static void clear_screen() {
	printf("\x1b[2J");
}

static void reset_cursor() {
	printf("\x1b[H");
}

static void wait(const uint16_t* const delay) {
	usleep((*delay ? *delay : DEFAULT_DELAY_MILLIS) * MICROS_PER_MILLI);
}

static void free_buffer(cell_t** buffer) {
	if (*buffer) {
		free(*buffer);
		*buffer = NULL;
	}
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
	if (result != ASCIIGOL_OK)
		free_buffer(cells);
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
	if (!*cells)
		return ASCIIGOL_BAD_DIMENSION;
	srand(time(NULL));
	for (uint16_t i = 0; i < size; i++)
		(*cells)[i] = (cell_t)(rand() % 2);
	return ASCIIGOL_OK;
}

static asciigol_result_t init_back_buffer(
	cell_t** back_buffer,
	const uint16_t size
) {
	*back_buffer = (cell_t*)malloc(size);
	if (!*back_buffer)
		return ASCIIGOL_BAD_DIMENSION;
	return ASCIIGOL_OK;
}

static asciigol_result_t init_cells(
	cell_t** cells,
	cell_t** back_buffer,
	uint8_t* const width,
	uint8_t* const height,
	char* const filename
) {
	asciigol_result_t result = ASCIIGOL_OK;
	if (filename)
		result = init_cells_from_file(cells, width, height, filename);
	else
		result = init_cells_at_random(cells, width, height);
	if (result != ASCIIGOL_OK)
		return result;
	result = init_back_buffer(back_buffer, *width * *height);
	if (result != ASCIIGOL_OK)
		free_buffer(cells);
	return result;
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
	int16_t row_begin = *row || *wrap ? *row - 1 : *row;
	int16_t col_begin = *col || *wrap ? *col - 1 : *col;
	int16_t row_end = *row < *height - 1 || *wrap ? *row + 1 : *row;
	int16_t col_end = *col < *width - 1 || *wrap ? *col + 1 : *col;
	uint8_t num_live_neighbors = 0;
	for (int16_t r = row_begin; r <= row_end; r++) {

		// account for wrap around
		uint8_t neighbor_row;
		if (r == -1)
			neighbor_row = *height - 1;
		else if (r == *height)
			neighbor_row = 0;
		else
			neighbor_row = (uint8_t)r;

		for (int16_t c = col_begin; c <= col_end; c++) {

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
	return compute_game_of_life(&cell, &num_live_neighbors);
}

static asciigol_result_t compute_cells(
	cell_t** cells,
	cell_t** new_cells,
	const uint8_t* const width,
	const uint8_t* const height,
	const bool* const wrap
) {
	bool converged = true;
	uint16_t size = *width * *height;
	for (uint16_t i = 0; i < size; i++) {
		uint8_t row = (uint8_t)(i / *width);
		uint8_t col = (uint8_t)(i % *width);
		cell_t cell = (*cells)[i];
		cell_t new_cell = compute_cell(cells, &row, &col, width, height, wrap);
		if (cell != new_cell)
			converged = false;
		(*new_cells)[i] = new_cell;
	}
	return converged ? ASCIIGOL_CONVERGED : ASCIIGOL_OK;
}

static void swap_buffers(cell_t** buffer_a, cell_t** buffer_b) {
	cell_t* temp = *buffer_a;
	*buffer_a = *buffer_b;
	*buffer_b = temp;
}

static void render_cells(
	cell_t** const cells,
	const uint8_t* const width,
	const uint8_t* const height,
	const char* const live_char,
	const char* const dead_char,
	const asciigol_bg_t* const background
) {
	uint16_t size = *width * *height;
	for (uint16_t i = 0; i < size; i++) {
		const bool is_live_cell = (bool)(*cells)[i];
		const bool are_chars_same =
			*live_char && *dead_char && *live_char == *dead_char;
		const bool alternate_bg = are_chars_same && !is_live_cell;
		const char character = is_live_cell ?
			(*live_char ? *live_char : DEFAULT_LIVE_CHAR) :
			(*dead_char ? *dead_char : DEFAULT_DEAD_CHAR);
		switch (*background) {
			case ASCIIGOL_BG_LIGHT:
				printf("%s", alternate_bg ? BG_BLACK_FG_WHITE : BG_WHITE_FG_BLACK);
				break;
			case ASCIIGOL_BG_DARK:
				printf("%s", alternate_bg ? BG_WHITE_FG_BLACK : BG_BLACK_FG_WHITE);
				break;
			case ASCIIGOL_BG_NONE:
			default:
				printf("%s", BG_DEFAULT_FG_DEFAULT);
		}
		putchar(character);
		if (i % *width == *width - 1)
			printf("%s\n", BG_DEFAULT_FG_DEFAULT);
	}
}

static void destroy_cells(cell_t** cells, cell_t** back_buffer) {
	free_buffer(cells);
	free_buffer(back_buffer);
}

