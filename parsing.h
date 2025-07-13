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
 * @brief Parses a 8-bit unsigned integer from string
 * @param arg The argument to parse
 * @param value Address to give the parsed uint8 to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_uint8(char* const arg, uint8_t* value);

/**
 * @brief Parses a 16-bit unsigned integer from string
 * @param arg The argument to parse
 * @param value Address to give the parsed uint16 to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_uint16(char* const arg, uint16_t* value);

/**
 * @brief Parses a character from a string
 * @param arg The argument to parse
 * @param character Address to give the parsed character to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_char(char* const arg, char* character);

/**
 * @brief Parses a string by handing it to another address
 * @param arg The argument to parse
 * @param string Address to give the parsed string to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_string(char* const arg, char** string);

/**
 * @brief Parses a boolean argument from string literal "true" or "false"
 * @param arg The argument to parse
 * @param boolean Address to give the parsed value to
 * @return true if parsing succeeded, false otherwise
 */
bool parse_bool(char* const arg, bool* boolean);

#endif // PARSING_H

