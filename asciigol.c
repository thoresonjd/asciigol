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

static char computeCell(char** cells, int row, int col, int width, int height) {
	char cell = (*cells)[width * row + col];
	int rowBegin = row ? row - 1 : row;
	int colBegin = col ? col - 1 : col;
	int rowEnd = row < height - 1 ? row + 1 : row;
	int colEnd = col < width - 1 ? col + 1 : col;
	int numNeighbors = 0;
	for (int i = rowBegin; i <= rowEnd; i++) {
		for (int j = colBegin; j <= colEnd; j++) {
			if (i == row && j == col)
				continue;
			int neighbor = (*cells)[width * i + j];
			if (neighbor)
				numNeighbors++;
		}
	}
	// GOL rules
	if (cell && numNeighbors < 2)
		return 0;
	if (cell && (numNeighbors == 2 || numNeighbors == 3))
		return 1;
	if (cell && numNeighbors > 3)
		return 0;
	if (!cell && numNeighbors == 3)
		return 1;
	return 0;
}

static void computeCells(char** cells, int width, int height) {
	int size = width * height;
	char* newCells = (char*)malloc(size);
	for (int i = 0; i < size; i++) {
		int row = i / width;
		int col = i % width;
		newCells[i] = computeCell(cells, row, col, width, height);
	}
	destroyCells(cells);
	*cells = newCells;
}

static void renderCells(char** cells, int width, int height) {
	int size = width * height;
	for (int i = 0; i < size; i++) {
		char character = (*cells)[i] ? LIVE_CELL_CHAR : DEAD_CELL_CHAR;
		putchar(character);
		if (i % width == width - 1)
			putchar('\n');
	}
}

void asciigol() {
	char* cells = NULL;
	initCells(&cells, GRID_WIDTH, GRID_HEIGHT);
	clearScreen();
	while (1) { // TODO: end game if/when cells converge and don't change
		resetCursor();
		computeCells(&cells, GRID_WIDTH, GRID_HEIGHT);
		renderCells(&cells, GRID_WIDTH, GRID_HEIGHT);
		wait();
	}
}

