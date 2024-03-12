#include "../inc/Parser.hpp"
#include <algorithm>
#include <sstream>
#include <string>

// trims line based on delimiters
//
// @param	str: string to trim
// @param	delimiters: string containing all delimiters
//
// @return	new trimmed string
std::string		Parser::trim(const std::string& str, const std::string& delimiters)
{
	size_t	first = str.find_first_not_of(delimiters);
	if (first == std::string::npos)
		return "";
	size_t	last = str.find_last_not_of(delimiters);
	return str.substr(first, (last - first + 1));
}

// splits line based on delmiters
//
// @param 	str: string to split
// @param 	delimiters: string containing all delimiters
//
// @return	vector of strings
std::vector <std::string>	Parser::split(const std::string& str, const std::string& delimiters)
{
	size_t						left = str.find_first_not_of(delimiters, 0), right = 0;
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

// splits line based on delimiters, storing quoted sequences as one word regardless of delimiters
//
// @param 	str: string to split
// @param 	delimiters: string containing all delimiters
//
// @return	vector of strings
std::vector <std::string>	Parser::split_keep_quoted_words(const std::string& str, const std::string& delimiters)
{
	bool						in_quotes = false;
	size_t						left = 0, right = 0;
	std::vector <std::string>	ret;

	while (right <= str.size())
	{
		if (right == str.size() || (in_quotes == false && delimiters.find(str[right]) != std::string::npos)) 
		{
                if (left != right) 
				{
                    ret.push_back(str.substr(left, right - left));
                }
                left = right + 1;
		}
		else if (str[right] == '"' && (right == 0 || str[right - 1] != '\\'))
		{
			in_quotes = !in_quotes;
			if (in_quotes == false)
			{
				ret.push_back(str.substr(left + 1, right - left - 1));
				left = right + 1;
			}
		}
		right++;
	}
	if (in_quotes == false && left < str.size())
	{
		ret.push_back(str.substr(left));
	}
	return ret;
}

std::string		Parser::trim_comment(const std::string& str, const std::string& delimiters)
{
	return str.substr(0, str.find_first_of(delimiters));
}

std::vector <std::pair <std::string, int> > Parser::split_keep_delimiters(const std::string& str, const std::string& delimiters)
{
	int 										line_number = 1, quote_number = 0;
	bool										in_quotes = false;
	size_t 										left = 0, right = 0;
	std::vector	<std::pair<std::string, int> > 	ret;

	while (right <= str.size()) 
	{
		if (str[right] == '"')
		{
			in_quotes = !in_quotes;
			quote_number++;
		}
		else if (in_quotes == false && (right == str.size() || delimiters.find(str[right]) != std::string::npos))
		{
			if (left < right)
			{
				std::string segment = str.substr(left, right - left);
				line_number += std::count(segment.begin(), segment.end(), '\n');
				if (Parser::trim(segment, "\t\n ") != "")
				{
					ret.push_back(std::make_pair(Parser::trim(segment, "\t\n "), line_number));
				}
			}
			if (right < str.size())
			{
				std::string delimiter(1, str[right]);
				if (delimiter != "\n" && delimiter.find_first_of("\t\n ") == std::string::npos)
					ret.push_back(std::make_pair(delimiter, line_number));
				line_number += std::count(delimiter.begin(), delimiter.end(), '\n');
			}	
			left = right + 1;
		}
		if (in_quotes == true && right == str.size() - 1)
		{
			std::string segment = str.substr(left, right - left);
			line_number += std::count(segment.begin(), segment.end(), '\n');
			if (Parser::trim(segment, "\t\n ") != "")
				ret.push_back(std::make_pair(Parser::trim(segment, "\t\n "), line_number));
		}
		right++;
	}
	if (quote_number % 2 != 0)
	{
		std::vector <std::pair <std::string, int> > error;
		error.push_back(std::make_pair(UNCLOSED_QUOTE, line_number));
		return error;
	}
	return ret;
}

std::string	Parser::itoa(int num)
{
	std::ostringstream	oss;

	oss << num;
	return oss.str();
}