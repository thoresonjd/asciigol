/**
 * @file parsing.c
 * @brief Parsing utilities
 * @author Justin Thoreson
 * @date 2025
 */

#include <parsing.h>
#include <stdio.h>
#include <string.h>

bool skip_prefix(char** string, const char* const prefix) {
	const size_t prefix_len = strlen(prefix);
	if (strncmp(*string, prefix, prefix_len))
		return false;
	*string += prefix_len;
	return true;
}

bool parse_uint8(const char* const arg, uint8_t* value) {
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

bool parse_uint16(const char* const arg, uint16_t* value) {
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

bool parse_char(const char* const arg, char* character) {
	if (!arg || !character)
		return false;
	if (strlen(arg) != 1)
		return false;
	if (!sscanf(arg, "%c", character))
		return false;
	return true;
}

