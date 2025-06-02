/**
 * @file asciigol.c
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// TODO: dimensions (and maybe sleep time?) should be dynamic at some point,
//		 hence the use of malloc bellow; for now, fixed values will suffice
const int GRID_WIDTH = 100;
const int GRID_HEIGHT = 40;
const int SLEEP_TIME_MILLIS = 50;
const int MICROS_PER_MILLI = 1000;
const char LIVE_CELL_CHAR = '#';
const char DEAD_CELL_CHAR = ' ';

void clearScreen() {
	printf("\x1b[2J");
}

void resetCursor() {
	printf("\x1b[H");
}

void wait() {
	usleep(SLEEP_TIME_MILLIS * MICROS_PER_MILLI);
}

void initCells(char** cells, int width, int height) {
	int size = width * height;
	*cells = (char*)malloc(size);
	// TODO: random initial state; allow fixed configurations as well
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		(*cells)[i] = rand() % 2;
}

void destroyCells(char** cells) { 
	free(*cells);
}

char computeCell(char** cells, int row, int col, int width, int height) {
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

void computeCells(char** cells, int width, int height) {
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

void renderCells(char** cells, int width, int height) {
	int size = width * height;
	for (int i = 0; i < size; i++) {
		char character = (*cells)[i] ? LIVE_CELL_CHAR : DEAD_CELL_CHAR;
		putchar(character);
		if (i % width == width - 1)
			putchar('\n');
	}
}

void gameOfLife() {
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

int main() {
	gameOfLife();
	return EXIT_SUCCESS;
}
