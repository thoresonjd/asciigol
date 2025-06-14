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

static void clearScreen() {
	printf("\x1b[2J");
}

static void resetCursor() {
	printf("\x1b[H");
}

static void wait(const unsigned int delay) {
	usleep((delay ? delay : DEFAULT_DELAY_MILLIS) * MICROS_PER_MILLI);
}

static enum AsciigolError initCellsFromFile(char** cells, unsigned int* const width, unsigned int* const height, char* const filename) {
	enum AsciigolError result = ASCIIGOL_OK;
	FILE* file = fopen(filename, "r");
	if (!file)
		result = ASCIIGOL_BADFILE;
	// read constant first line: "asciigol"
	char* line = NULL;
	size_t lineLen = 0;
	if (getline(&line, &lineLen, file) <= 0) {
		result = ASCIIGOL_BADHEADER;
		goto EXIT;
	}
	if (strcmp(line, "asciigol\n")) {
		result = ASCIIGOL_BADHEADER;
		goto EXIT;
	}
	// read width and height
	free(line);
	line = NULL;
	if (getline(&line, &lineLen, file) <= 0) {
		result = ASCIIGOL_BADDIMENSION;
		goto EXIT;
	}
	int tempWidth, tempHeight;
	if (sscanf(line, "%d,%d", &tempWidth, &tempHeight) < 2) {
		result = ASCIIGOL_BADDIMENSION;
		goto EXIT;
	}
	if (tempWidth <= 0 || tempWidth > MAX_WIDTH || tempHeight <= 0 || tempHeight > MAX_HEIGHT) {
		result = ASCIIGOL_BADDIMENSION;
		goto EXIT;
	}
	*width = (unsigned int)tempWidth;
	*height = (unsigned int)tempHeight;
	// read initial cell states
	unsigned int size = *width * *height;
	*cells = (char*)malloc(size);
	if (!cells) {
		result = ASCIIGOL_BADDIMENSION; // TODO: change to different error
		goto EXIT;
	}
	char c;
	unsigned int index = 0, widthCount = 0, heightCount = 0;
	while((c = getc(file)) != EOF) {
		if (heightCount >= *height) {
			result = ASCIIGOL_BADDIMENSION;
			goto EXIT;
		}
		if (c == '\n') {
			if (widthCount < *width) {
				result = ASCIIGOL_BADDIMENSION;
				goto EXIT;
			}
			heightCount++;
			widthCount = 0;
			continue;
		}
		if (c != '0' && c != '1') {
			result = ASCIIGOL_BADCELL;
			goto EXIT;
		}
		if (++widthCount > *width) {
			result = ASCIIGOL_BADDIMENSION;
			goto EXIT;
		}
		(*cells)[index++] = c - '0'; // literal integer 1 or 0, not ASCII value
	}
	if (heightCount < *height) {
		result = ASCIIGOL_BADDIMENSION;
		goto EXIT;
	}

EXIT:
	if (line)
		free(line);
	if (*cells)
		free(*cells);
	if (file)
		fclose(file);
	return result;
}

static enum AsciigolError initCellsAtRandom(char** cells, unsigned int* const width, unsigned int* const height) {
	if (*width > MAX_WIDTH || *height > MAX_HEIGHT)
		return ASCIIGOL_BADDIMENSION;
	*width = *width ? *width : DEFAULT_WIDTH;
	*height = *height ? *height : DEFAULT_HEIGHT;
	int size = *width * *height;
	*cells = (char*)malloc(size);
	if (!cells)
		return ASCIIGOL_BADDIMENSION; // TODO: change to different error
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		(*cells)[i] = rand() % 2;
	return ASCIIGOL_OK;
}

static enum AsciigolError initCells(char** cells, unsigned int* const width, unsigned int* const height, char* const filename) {
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

static void renderCells(char** cells, const int width, const int height, const char liveChar, const char deadChar) {
	int size = width * height;
	for (int i = 0; i < size; i++) {
		const char character = (*cells)[i] ?
				(liveChar ? liveChar : DEFAULT_LIVE_CHAR) :
				(deadChar ? deadChar : DEFAULT_DEAD_CHAR);
		putchar(character);
		if (i % width == width - 1)
			putchar('\n');
	}
}

void asciigol(struct AsciigolArgs args) {
	char* cells = NULL;
	enum AsciigolError result = initCells(&cells, &args.width, &args.height, args.filename);
	if (result) {
		// TODO: avoid printing here and return the error code instead
		printf("Failed to initialize cells - error number %d\n", result);
		return;
	}
	clearScreen();
	while (true) { // TODO: end game if/when cells converge and don't change
		resetCursor();
		renderCells(&cells, args.width, args.height, args.liveChar, args.deadChar);
		computeCells(&cells, args.width, args.height, args.wrapAround);
		wait(args.delay);
	}
}

