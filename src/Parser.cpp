#include "../inc/Parser.hpp"

// trims server config delimiters off line
std::string		Parser::trim(const std::string &str)
{
	size_t	first = str.find_first_not_of(" \t\n;{}");
	if (first == std::string::npos)
		return "";
	size_t	last = str.find_last_not_of(" \t\n;{}");
	return str.substr(first, (last - first + 1));
}