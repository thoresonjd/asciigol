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

struct AsciigolArgs {
	unsigned int width;
	unsigned int height;
	unsigned int delay;
	char* filename;
	char liveChar;
	char deadChar;
	bool wrapAround;
};

enum AsciigolError {
	ASCIIGOL_OK = 0,
	ASCIIGOL_BADFILE = 1,
	ASCIIGOL_BADHEADER = 2,
	ASCIIGOL_BADDIMENSION = 3,
	ASCIIGOL_BADCELL = 4,
};

void asciigol(struct AsciigolArgs args);

#endif // ASCIIGOL_H

