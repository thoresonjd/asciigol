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
#include <stdint.h>

/**
 * @brief Arguments to be given to the asciigol program.
 */
typedef struct {
	uint8_t width;
	uint8_t height;
	uint16_t delay;
	char* filename;
	char live_char;
	char dead_char;
	bool wrap;
} asciigol_args_t;

/**
 * @brief Result codes returned by the asciigol program.
 */
typedef enum {
	ASCIIGOL_OK,
	ASCIIGOL_CONVERGED,
	ASCIIGOL_BAD_FILE,
	ASCIIGOL_BAD_HEADER,
	ASCIIGOL_BAD_DIMENSION,
	ASCIIGOL_BAD_CELL,
} asciigol_result_t;

/**
 * @brief Execute ASCII Game of Life.
 * @param[in] args A structure of arguments to configure asciigol with.
 * @return An enum denoting the asciigol result code.
 */
asciigol_result_t asciigol(asciigol_args_t args);

#endif // ASCIIGOL_H

