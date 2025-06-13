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
static const char DEFAULT_LIVE_CELL_CHAR = '#';
static const char DEAD_CELL_CHAR = ' ';

static void clearScreen() {
	printf("\x1b[2J");
}

static void resetCursor() {
	printf("\x1b[H");
}

static void wait(const unsigned int delay) {
	usleep((delay ? delay : DEFAULT_DELAY_MILLIS) * MICROS_PER_MILLI);
}

// TODO: cleanup if and where possible
static bool initCellsFromFile(char** cells, unsigned int* const width, unsigned int* const height, char* const filename) {
	FILE* file = fopen(filename, "r");
	if (!file)
		return false;
	// read constant first line: "asciigol"
	char* line = NULL;
	size_t lineLen = 0;
	if (getline(&line, &lineLen, file) <= 0) {
		fclose(file);
		return false;
	}
	if (strcmp(line, "asciigol\n")) {
		fclose(file);
		return false;
	}
	// read width and height
	if (getline(&line, &lineLen, file) <= 0) {
		fclose(file);
		return false;
	}
	int tempWidth, tempHeight;
	if (sscanf(line, "%d,%d", &tempWidth, &tempHeight) < 2) {
		fclose(file);
		return false;
	}
	if (tempWidth <= 0 || tempWidth > MAX_WIDTH || tempHeight <= 0 || tempHeight > MAX_HEIGHT) {
		fclose(file);
		return false;
	}
	*width = (unsigned int)tempWidth;
	*height = (unsigned int)tempHeight;
	// read initial cell states
	unsigned int size = *width * *height;
	*cells = (char*)malloc(size);
	if (!cells) {
		fclose(file);
		return false;
	}
	char c;
	unsigned int index = 0, widthCount = 0, heightCount = 0;
	while((c = getc(file)) != EOF) {
		if (heightCount >= *height) {
			free(*cells);
			fclose(file);
			return false;
		}
		if (c == '\n') {
			if (widthCount < *width) {
				free(*cells);
				fclose(file);
				return false;
			}
			heightCount++;
			widthCount = 0;
			continue;
		}
		if (c != '0' && c != '1') {
			free(*cells);
			fclose(file);
			return false;
		}
		if (++widthCount > *width) {
			free(*cells);
			fclose(file);
			return false;
		}
		(*cells)[index++] = c - '0'; // literal integer 1 or 0, not ASCII value
	}
	if (heightCount < *height) {
		free(*cells);
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}

static bool initCellsAtRandom(char** cells, unsigned int* const width, unsigned int* const height) {
	if (*width > MAX_WIDTH || *height > MAX_HEIGHT)
		return false;
	if (!*width)
		*width = DEFAULT_WIDTH;
	if (!*height)
		*height = DEFAULT_HEIGHT;
	int size = *width * *height;
	*cells = (char*)malloc(size);
	if (!cells)
		return false;
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		(*cells)[i] = rand() % 2;
	return true;
}

static bool initCells(char** cells, unsigned int* const width, unsigned int* const height, char* const filename) {
	if (filename)
		return initCellsFromFile(cells, width, height, filename);
	return initCellsAtRandom(cells, width, height);
}

static char computeCell(char** cells, int row, int col, int width, int height, bool wrap) {
	char cell = (*cells)[width * row + col];
	// get neighbor range; will go out of grid range when wrap is enabled
	int rowBegin = row || wrap ? row - 1 : row;
	int colBegin = col || wrap ? col - 1 : col;
	int rowEnd = row < height - 1 || wrap ? row + 1 : row;
	int colEnd = col < width - 1 || wrap ? col + 1 : col;
	int numLiveNeighbors = 0;
	for (int r = rowBegin; r <= rowEnd; r++) {

		// account for wrap around
		int neighborRow = r;
		if (neighborRow == -1)
			neighborRow = height - 1;
		else if (neighborRow == height)
			neighborRow = 0;

		for (int c = colBegin; c <= colEnd; c++) {

			// account for wrap around
			int neighborCol = c;
			if (neighborCol == -1)
				neighborCol = width - 1;
			else if (neighborCol == width)
				neighborCol = 0;

			// count neighbor if live
			int neighbor = (*cells)[width * neighborRow + neighborCol];
			if (neighbor && (neighborRow != row || neighborCol != col))
				numLiveNeighbors++;
		}
	}
	// GOL rules
	if (cell && numLiveNeighbors < 2)
		return 0;
	if (cell && (numLiveNeighbors == 2 || numLiveNeighbors == 3))
		return 1;
	if (cell && numLiveNeighbors > 3)
		return 0;
	if (!cell && numLiveNeighbors == 3)
		return 1;
	return 0;
}

static void computeCells(char** cells, int width, int height, bool wrap) {
	int size = width * height;
	char* newCells = (char*)malloc(size);
	for (int i = 0; i < size; i++) {
		int row = i / width;
		int col = i % width;
		newCells[i] = computeCell(cells, row, col, width, height, wrap);
	}
	free(*cells);
	*cells = newCells;
}

static void renderCells(char** cells, int width, int height, char character) {
	int size = width * height;
	for (int i = 0; i < size; i++) {
		putchar((*cells)[i] ? character ? character : DEFAULT_LIVE_CELL_CHAR : DEAD_CELL_CHAR);
		if (i % width == width - 1)
			putchar('\n');
	}
}

void asciigol(struct AsciigolArgs args) {
	char* cells = NULL;
	if (!initCells(&cells, &args.width, &args.height, args.filename)) {
		// TODO: avoid printing here and return some error code instead
		printf("Failed to initialize cells\n");
		return;
	}
	clearScreen();
	while (true) { // TODO: end game if/when cells converge and don't change
		resetCursor();
		renderCells(&cells, args.width, args.height, args.character);
		computeCells(&cells, args.width, args.height, args.wrapAround);
		wait(args.delay);
	}
}

