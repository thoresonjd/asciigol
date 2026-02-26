/**
 * @file parsing.h
 * @brief Parsing utilities
 * @author Justin Thoreson
 * @date 2025
 */

#pragma once
#ifndef PARSING_H
#define PARSING_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Move a string pointer forward passed a specified prefix.
 * @param[in,out] string The string in which the prefix is skipped.
 * @param[in] prefix The prefix to skip.
 * @return True if parsing succeeded, false otherwise.
 */
bool skip_prefix(char** string, const char* prefix);

/**
 * @brief Parse a 8-bit unsigned integer from string.
 * @param[in] arg The argument to parse.
 * @param[out] value The parsed uint8.
 * @return True if parsing succeeded, false otherwise.
 */
bool parse_uint8(const char* const arg, uint8_t* value);

/**
 * @brief Parse a 16-bit unsigned integer from string.
 * @param[in] arg The argument to parse.
 * @param[out] value The parsed uint16.
 * @return True if parsing succeeded, false otherwise.
 */
bool parse_uint16(const char* const arg, uint16_t* value);

/**
 * @brief Parse a character from a string.
 * @param[in] arg The argument to parse.
 * @param[out] character The parsed char.
 * @return True if parsing succeeded, false otherwise.
 */
bool parse_char(const char* const arg, char* character);

#endif // PARSING_H

