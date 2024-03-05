#include "../inc/Parser.hpp"

// trims line based on delimiters
// @param str: string to trim
// @param delimiters: string containing all delimiters
// @return: new trimmed string
std::string		Parser::trim(const std::string& str, const std::string& delimiters)
{
	size_t	first = str.find_first_not_of(delimiters);
	if (first == std::string::npos)
		return "";
	size_t	last = str.find_last_not_of(delimiters);
	return str.substr(first, (last - first + 1));
}

// splits line based on delmiters
// @param str: string to split
// @param delimiters: string containing all delimiters
// @return: vector of strings
std::vector <std::string>	Parser::split(const std::string& str, const std::string& delimiters)
{
	size_t						left = str.find_first_not_of(delimiters, 0);
	size_t						right;
	std::vector <std::string>	ret;
	
	while (left != std::string::npos)
	{
		right = str.find_first_of(delimiters, left);
		if (right == std::string::npos)
		{
			if (left != std::string::npos)
				ret.push_back(str.substr(left));
			break ;
		}
		ret.push_back(str.substr(left, right - left));
		left = str.find_first_not_of(delimiters, right + 1);
	}
	return ret;
}