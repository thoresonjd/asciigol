/**
 * @file asciigol.h
 * @brief Conway's Game of Life animated as ASCII in the terminal.
 * @author Justin Thoreson
 * @date 2025
 */

#pragma once
#ifndef ASCIIGOL_H
#define ASCIIGOL_H

#include <stdbool.h>

typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int delay;
	char* filename;
	char live_char;
	char dead_char;
	bool wrap;
} asciigol_args_t;

typedef enum {
	ASCIIGOL_OK,
	ASCIIGOL_CONVERGED,
	ASCIIGOL_BAD_FILE,
	ASCIIGOL_BAD_HEADER,
	ASCIIGOL_BAD_DIMENSION,
	ASCIIGOL_BAD_CELL,
} asciigol_result_t;

asciigol_result_t asciigol(asciigol_args_t args);

#endif // ASCIIGOL_H

