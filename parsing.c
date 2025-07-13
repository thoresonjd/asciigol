/**
 * @file parsing.c
 * @brief Parsing utilities
 * @author Justin Thoreson
 * @date 2025
 */

#include "parsing.h"
#include <stdio.h>
#include <string.h>

bool parse_uint8(char* const arg, uint8_t* value) {
	if (!arg || !value)
		return false;
	int64_t temp_value;
	if (!sscanf(arg, "%ld", &temp_value))
		return false;
	if (temp_value < 0 || temp_value > UINT8_MAX)
		return false;
	*value = (uint8_t)temp_value;
	return true;
}

bool parse_uint16(char* const arg, uint16_t* value) {
	if (!arg || !value)
		return false;
	int64_t temp_value;
	if (!sscanf(arg, "%ld", &temp_value))
		return false;
	if (temp_value < 0 || temp_value > UINT16_MAX)
		return false;
	*value = (uint16_t)temp_value;
	return true;
}

bool parse_char(char* const arg, char* character) {
	if (!arg || !character)
		return false;
	if (!sscanf(arg, "%c", character))
		return false;
	return true;
}

bool parse_string(char* const arg, char** string) {
	if (!arg || !string)
		return false;
	(*string) = arg;
	return true;
}

bool parse_bool(char* const arg, bool* boolean) {
	if (!arg || !boolean)
		return false;
	char* temp_string;
	if (!parse_string(arg, &temp_string))
		return false;
	if (!strcmp(temp_string, "true"))
		(*boolean) = true;
	else if (!strcmp(temp_string, "false"))
		(*boolean) = false;
	else
		return false;
	return true;
}
