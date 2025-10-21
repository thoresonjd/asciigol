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
 * @brief Moves a string pointer forward passed a specified prefix
 * @param string The string in which the prefix is skipped
 * @param prefix The prefix to skip
 */
bool skip_prefix(char** string, const char* prefix);

/**
 * @brief Parses a 8-bit unsigned integer from string
 * @param arg The argument to parse
 * @param value Address to give the parsed uint8 to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_uint8(const char* const arg, uint8_t* value);

/**
 * @brief Parses a 16-bit unsigned integer from string
 * @param arg The argument to parse
 * @param value Address to give the parsed uint16 to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_uint16(const char* const arg, uint16_t* value);

/**
 * @brief Parses a character from a string
 * @param arg The argument to parse
 * @param character Address to give the parsed character to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_char(const char* const arg, char* character);

#endif // PARSING_H

