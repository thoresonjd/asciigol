/**
 * @file asciigolgen.h
 * @brief Configuration file generator for asciigol.
 * @author Justin Thoreson
 * @date 2025
 */

#include <stdint.h>

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
 * @param[in] filename The name of the file to write the config to.
 * @param[in] width The width of the asciigol grid.
 * @param[in] height The height of the asciigol grid.
 * @return An enum denoting the asciigolgen result code.
 */
asciigolgen_result_t asciigolgen(
	char* const filename,
	const uint8_t* const width,
	const uint8_t* const height);
