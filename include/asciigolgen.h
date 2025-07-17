/**
 * @file asciigolgen.h
 * @brief Configuration file generator for asciigol.
 * @author Justin Thoreson
 * @date 2025
 */

#pragma once
#ifndef ASCIIGOLGEN_H
#define ASCIIGOLGEN_H

#include <stdint.h>

/**
 * @brief Arguments to be given to the asciigolgen program.
 */
typedef struct {
	uint8_t width;
	uint8_t height;
	char* filename;
	char cell;
} asciigolgen_args_t;

/**
 * @brief Result codes returned by asciigolgen.
 */
typedef enum {
	ASCIIGOLGEN_OK,
	ASCIIGOLGEN_DONE,
	ASCIIGOLGEN_INVAL,
	ASCIIGOLGEN_FAIL
} asciigolgen_result_t;

/**
 * @brief Generates an asciigol configuration file.
 * @param[in] args A structure of arguments to configure asciigolgen with.
 * @return An enum denoting the asciigolgen result code.
 */
asciigolgen_result_t asciigolgen(asciigolgen_args_t args);

#endif // ASCIIGOLGEN_H

