/**
 * @file asciigol.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include "asciigol.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// TODO: dimensions (and maybe sleep time?) should be dynamic at some point,
//		 hence the use of malloc bellow; for now, fixed values will suffice
static const int GRID_WIDTH = 100;
static const int GRID_HEIGHT = 40;
static const int SLEEP_TIME_MILLIS = 50;
static const int MICROS_PER_MILLI = 1000;
static const char LIVE_CELL_CHAR = '#';
static const char DEAD_CELL_CHAR = ' ';

static void clearScreen() {
	printf("\x1b[2J");
}

static void resetCursor() {
	printf("\x1b[H");
}

static void wait() {
	usleep(SLEEP_TIME_MILLIS * MICROS_PER_MILLI);
}

static void initCells(char** cells, int width, int height) {
	int size = width * height;
	*cells = (char*)malloc(size);
	// TODO: random initial state; allow fixed configurations as well
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		(*cells)[i] = rand() % 2;
}

static void destroyCells(char** cells) { 
	free(*cells);
}

static char computeCell(char** cells, int row, int col, int width, int height, bool wrap) {
	char cell = (*cells)[width * row + col];
	// get neighbor range; will go out of range when wrap is enabled
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
	destroyCells(cells);
	*cells = newCells;
}

static void renderCells(char** cells, int width, int height) {
	int size = width * height;
	for (int i = 0; i < size; i++) {
		putchar((*cells)[i] ? LIVE_CELL_CHAR : DEAD_CELL_CHAR);
		if (i % width == width - 1)
			putchar('\n');
	}
}

void asciigol(const struct AsciigolArgs* const args) {
	char* cells = NULL;
	initCells(&cells, GRID_WIDTH, GRID_HEIGHT);
	clearScreen();
	while (true) { // TODO: end game if/when cells converge and don't change
		resetCursor();
		computeCells(&cells, GRID_WIDTH, GRID_HEIGHT, args->wrapAround);
		renderCells(&cells, GRID_WIDTH, GRID_HEIGHT);
		wait();
	}
}

