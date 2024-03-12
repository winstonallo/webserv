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
// @param str: 			string to split
// @param delimiters: 	string containing all delimiters
//
// @return:				vector of strings
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

// removes comments from a line based on the delimiters string
//
// @param str:			line to remove comments from
// @param delimiters:	delimiters signifying the beginning of a commment
std::string		Parser::trim_comment(const std::string& str, const std::string& delimiters)
{
	return str.substr(0, str.find_first_of(delimiters));
}

// helper function for 'split_keep_delimiters'
// 
// 1.	creates a substring from pos left to pos right
// 2.	updates the line count
// 3.	if resulting string is not empty, add to the vector with the corresponding line number
//
// @param str:			config file as a string
// @param ret:			vector to store the values into
// @param left:			start position of the substring
// @param right:		end position of the substring
// @param line_number:	current line_number to update & add to the vector 
void	get_pair(const std::string& str, std::vector <std::pair <std::string, int> >& ret, size_t left, size_t right, int& line_number)
{
	std::string segment = str.substr(left, right);
	line_number += std::count(segment.begin(), segment.end(), '\n');
	if (Parser::trim(segment, "\t\n ") != "")
	{
		ret.push_back(std::make_pair(Parser::trim(segment, "\t\n "), line_number));
	}
}

// this function is ugly af sorry
// splits a string based on a delimiter string, adding the delimiters themselves to the resulting vector,
// taking double quotes into account & keeping track of the line count
//
// @param str:			config file as a string
// @param delimiters:	delimiters to split the string by
// @return:				a vector of pairs:
//							- pair.first (string): 	the line we extracted
//							- pair.second (int):	the corresponding line number in the file
//
// - loops over the string, keeping track of the current quote status
//		1. if we encounter quotes, flip the quote status -> while in quotes, delimiters are ignored!
//		2. else if we find a delimiter, add the line & the delimiter to the vector
//		3. edge case handling for the end of the string 
// 
// - after the loop, if still in quotes: unclosed quotes -> throw error 
std::vector <std::pair <std::string, int> > Parser::split_keep_delimiters(const std::string& str, const std::string& delimiters)
{
	int 										line_number = 1;
	bool										in_quotes = false;
	size_t 										left = 0, right = 0;
	std::vector	<std::pair<std::string, int> > 	ret;

	while (right <= str.size()) 
	{
		if (str[right] == '"')
		{
			in_quotes = !in_quotes;
		}
		else if (in_quotes == false && (right == str.size() || delimiters.find(str[right]) != std::string::npos))
		{
			if (left < right)
			{
				get_pair(str, ret, left, right - left, line_number);
			}
			if (right < str.size())
			{
				std::string delimiter(1, str[right]);
				ret.push_back(std::make_pair(delimiter, line_number));
				line_number += std::count(delimiter.begin(), delimiter.end(), '\n');
			}	
			left = right + 1;
		}
		if (in_quotes == true && right == str.size() - 1)
		{
			get_pair(str, ret, left, right - left, line_number);
		}
		right++;
	}
	if (in_quotes == true)
	{
		std::vector <std::pair <std::string, int> > error;
		error.push_back(std::make_pair(UNCLOSED_QUOTE, line_number));
		return error;
	}
	return ret;
}

// convert int to string
std::string	Parser::itoa(int num)
{
	std::ostringstream	oss;

	oss << num;
	return oss.str();
}