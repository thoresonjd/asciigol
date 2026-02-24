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

/**
 * @brief The data type representing a Game of Life cell.
 */
typedef char cell_t;

/**
 * @brief Directions of movement within the terminal.
 *
 * The value of each direction corresponds to the ANSI control sequence that
 * moves the cursor in said direction, which also corresponds to the value of
 * the respective arrow key on the keyboard.
 */
typedef enum {
	DIRECTION_UP = 'A',
	DIRECTION_DOWN = 'B',
	DIRECTION_RIGHT = 'C',
	DIRECTION_LEFT = 'D'
} direction_t;

/**
 * @brief The representation of a live cell.
 */
static const cell_t LIVE_CELL = '1';

/**
 * @brief The representation of a dead cell.
 */
static const cell_t DEAD_CELL = '0';

/**
 * @brief The character indicating that the program should terminate.
 */
static const char QUIT = 'q';

/**
 * @brief User instructions for controlling the program.
 */
static const char* CONTROLS =
	"Move: Up, Down, Left, Right\n"
	"Modify: 0, 1\n"
	"Quit: q";

/**
 * @brief Retrieve the current terminal settings.
 * @return The parameters of the current terminal.
 */
static struct termios get_terminal();

/**
 * @brief Set the attributes of the current terminal.
 * @param[in] terminal The terminal parameters to set. 
 */
static void set_terminal(const struct termios* terminal);

/**
 * @brief Enable noncanonical mode/disable canonical mode and disable the
 *        echoing for the given terminal attributes.
 * @param[in] terminal The provided terminal settings to modify.
 * @return A copy of the provided terminal settings in noncanonical mode.
 */
static struct termios terminal_noncanon(struct termios terminal);

/**
 * @brief Clear the contents of the terminal screen.
 */
static void clear_screen();

/**
 * @brief Reset the cursor to the top-left position.
 */
static void reset_cursor();

/**
 * @brief Initialize the state of the Game of Life generator.
 * @param[out] state The initialized state.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] cell The value to initialize all cells.
 * @return The result of the initialization.
 */
static asciigolgen_result_t init_state(
	cell_t** const state,
	const uint8_t width,
	const uint8_t height,
	const cell_t cell
);

/**
 * @brief Render the state of the Game of Life grid.
 * @param[in] state The Game of Life state to render.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] highlight_idx The index of the Game of Life cell to highlight.
 * @return The result of printing the Game of Life state.
 */
static asciigolgen_result_t print_state(
	cell_t* const state,
	const uint8_t width,
	const uint8_t height,
	const uint16_t highlight_idx
);

/**
 * @brief Process keyboard input.
 * @param[in,out] state The Game of Life state to process.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in,out] highlight_idx The index of the Game of Life cell to highlight.
 * @return The result of processing input.
 */
static asciigolgen_result_t process_input(
	cell_t* state,
	const uint8_t width,
	const uint8_t height,
	uint16_t* const highlight_idx
);

/**
 * @brief Modify the Game of Life state.
 * @param[in,out] state The Game of Life state to modify.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in,out] highlight_idx The index of the Game of Life cell to highlight.
 * @return The result of modifying the state.
 */
static asciigolgen_result_t modify_state(
	cell_t* const state,
	const uint8_t width,
	const uint8_t height
);

/**
 * @brief Write the Game of Life state to a configuration file.
 * @param[in] filename The name of a file to write the configuration to.
 * @param[in] state The Game of Life state to modify.
 * @param[in] width The width of the Game of Life grid.
 * @param[in] height The height of the Game of Life grid.
 * @param[in] highlight_idx The index of the Game of Life cell to highlight.
 * @return The result of writing the state to the configuration file.
 */
static asciigolgen_result_t write_config(
	char* const filename,
	cell_t* const state,
	const uint8_t width,
	const uint8_t height
);

asciigolgen_result_t asciigolgen(asciigolgen_args_t args) {
	cell_t* state = NULL;
	asciigolgen_result_t result = init_state(&state, args.width, args.height, args.cell);
	if (result != ASCIIGOLGEN_OK)
		return result;
	struct termios orig_terminal = get_terminal();
	struct termios new_terminal = terminal_noncanon(orig_terminal);
	set_terminal(&new_terminal);
	result = modify_state(state, args.width, args.height);
	if (result == ASCIIGOLGEN_OK || result == ASCIIGOLGEN_DONE)
		result = write_config(args.filename, state, args.width, args.height);
	set_terminal(&orig_terminal);
	if (state) {
		free(state);
		state = NULL;
	}
	return result;
}

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
	const uint8_t width,
	const uint8_t height,
	const cell_t cell
) {
	if (cell != DEAD_CELL && cell != LIVE_CELL)
		return ASCIIGOLGEN_INVAL;
	const uint16_t size = width * height;
	*state = (cell_t*)malloc(size);
	if (!*state)
		return ASCIIGOLGEN_FAIL;
	for (uint16_t i = 0; i < size; i++)
		(*state)[i] = cell;
	return ASCIIGOLGEN_OK;
}

static asciigolgen_result_t print_state(
	cell_t* const state,
	const uint8_t width,
	const uint8_t height,
	const uint16_t highlight_idx
) {
	if (!state)
		return ASCIIGOLGEN_INVAL;
	const uint16_t size = width * height;
	for (uint16_t i = 0; i < size; i++) {
		if (i == highlight_idx)
			printf("\x1b[32m%c\x1b[0m", state[i]);
		else 
			putchar(state[i]);
		if (i % width == width - 1)
			putchar('\n');
	}
	printf("\n%s\n", CONTROLS);
	return ASCIIGOLGEN_OK;
}

static asciigolgen_result_t process_input(
	cell_t* const state,
	const uint8_t width,
	const uint8_t height,
	uint16_t* const highlight_idx
) {
	if (!state || !highlight_idx)
		return ASCIIGOLGEN_INVAL;
	char c;
	if (read(STDIN_FILENO, &c, 1) <= 0)
		return ASCIIGOLGEN_FAIL;
	if (c == QUIT)
		return ASCIIGOLGEN_DONE;
	else if (c == LIVE_CELL || c == DEAD_CELL)
		state[*highlight_idx] = c;
	else if (c == '\x1b') { // ANSI escape code
		getchar(); // skip [
		char value = getchar();
		const uint16_t size = width * height;
		if (value == DIRECTION_UP && *highlight_idx >= width)
			(*highlight_idx) -= width;
		else if (value == DIRECTION_DOWN && *highlight_idx < size - width)
			(*highlight_idx) += width;
		else if (value == DIRECTION_RIGHT && *highlight_idx < size - 1)
			(*highlight_idx)++;
		else if (value == DIRECTION_LEFT && *highlight_idx > 0)
			(*highlight_idx)--;
	}
	return ASCIIGOLGEN_OK;
}

static asciigolgen_result_t modify_state(
	cell_t* const state,
	const uint8_t width,
	const uint8_t height
) {
	if (!state)
		return ASCIIGOLGEN_INVAL;
	uint16_t highlight_idx = 0;
	asciigolgen_result_t result = ASCIIGOLGEN_OK;
	clear_screen();
	do {
		reset_cursor();
		result = print_state(state, width, height, highlight_idx);
		if (result != ASCIIGOLGEN_OK)
			return result;
		result = process_input(state, width, height, &highlight_idx);
	} while (result == ASCIIGOLGEN_OK);
	return result;
}


static asciigolgen_result_t write_config(
	char* const filename,
	cell_t* const state,
	const uint8_t width,
	const uint8_t height
) {
	if (!filename || !state)
		return ASCIIGOLGEN_INVAL;
	FILE* file = fopen(filename, "w");
	if (!file)
		return ASCIIGOLGEN_FAIL;
	fwrite("asciigol\n", sizeof("asciigol\n") - 1, 1, file);
	fprintf(file, "%u,%u\n", width, height);
	const uint16_t size = width * height;
	for (uint16_t i = 0; i < size; i++) {
		fputc(state[i], file);
		if (i % width == width - 1)
			fputc('\n', file);
	}
	fclose(file);
	return ASCIIGOLGEN_OK;
}

