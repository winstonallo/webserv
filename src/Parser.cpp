#include "../inc/Parser.hpp"

// trims server config delimiters off line
std::string		Parser::trim(const std::string& str)
{
	size_t	first = str.find_first_not_of(" \t\n;{}");
	if (first == std::string::npos)
		return "";
	size_t	last = str.find_last_not_of(" \t\n;{}");
	return str.substr(first, (last - first + 1));
}

std::vector <std::string>	Parser::split(const std::string& str, const std::string& delimiters)
{
	size_t						left = str.find_first_not_of(delimiters, 0);
	std::vector <std::string>	ret;
	
	while (true)
	{
		size_t	right = str.find_first_of(delimiters, left);
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