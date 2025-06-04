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
	char character;
	bool wrapAround;
};

void asciigol(struct AsciigolArgs args);

#endif // ASCIIGOL_H

