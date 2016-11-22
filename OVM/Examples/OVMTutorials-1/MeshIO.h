#pragma once
#include <fstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#define _INPUTLINESIZE_ 4096

inline char* read_line_chars(char* _result, std::ifstream& _infile, int& _linenumber)
{
	do
	{
		if (_infile.eof())
		{
			return (char *)NULL;
		}
		//result = fgets(_str, (int)_INPUTLINESIZE_ - 1, _infile);
		_infile.getline(_result, (int)_INPUTLINESIZE_);
		_linenumber++;
		// Search for a non-empty line.
		// Skip white spaces.
		while ((*_result == ' ') || (*_result == '\t'))
		{
			_result++;
		}
		// If it's end of line, read another line and try again.
	} while (*_result == '\0');
	return _result;
}

inline char* find_next_sub_str(char* _str)
{
	char * result;

	result = _str;
	// Skip the current field.  Stop upon reaching whitespace or a comma.
	while ((*result != '\0') &&
		(*result != '#') &&
		(*result != ' ') &&
		(*result != '\t') &&
		(*result != ','))
	{
		result++;
	}
	// Now skip the whitespace and anything else that doesn't look like a
	//   number, a comment, or the end of a line. 
	while ((*result != '\0') &&
		(*result != '#') &&
		(*result != '.') &&
		(*result != '+') &&
		(*result != '-') &&
		((*result < '0') || (*result > '9'))
		)
	{
		result++;
	}
	// Check for a comment (prefixed with `#').
	if (*result == '#') {
		*result = '\0';
	}
	return result;
}

inline char* trim_str(char* _str)
{
	char * result;
	result = _str;
	char * reverse;
	while ((*result == ' ') ||
		(*result == '\t'))
	{
		++result;
	}
	reverse = _str + strlen(_str) - 1;
	while ((*reverse == ' ') ||
		(*reverse == '\t'))
	{
		--reverse;
	}
	++reverse;
	*reverse = '\0';
	return result;
}