#include "../inc/Parser.hpp"
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

std::string		Parser::trim_comment(const std::string& str, const std::string& delimiters)
{
	return str.substr(0, str.find_first_of(delimiters));
}

// splits a string by delimiters, also adding the delimiters into the vector as their own strings
std::vector <std::pair <std::string, int> > Parser::split_keep_delimiters(const std::string& str, const std::string& delimiters) {
        size_t left = 0;
        size_t right;
        std::vector< std::pair<std::string, int> > ret;
        int line_number = 0;

        while (left < str.size()) {
            right = str.find_first_of(delimiters, left);

            if (right != std::string::npos) {
                std::string segment = str.substr(left, right - left);
                // Check the segment for newlines and adjust line_number accordingly
                for(size_t i = 0; i < segment.length(); ++i) {
                    if(segment[i] == '\n') {
                        ++line_number;
                    }
                }

                if (!segment.empty()) {
                    std::string content = Parser::trim(segment, "\t\n ");
                    if (!content.empty())
                        ret.push_back(std::make_pair(content, line_number));
                }
                // Handle the delimiter itself
                std::string delimiter(1, str[right]);
                if (!delimiter.empty()) {
                    // Increment line_number if the delimiter is a newline
                    if(delimiter == "\n") {
                        ++line_number;
                    }
                    ret.push_back(std::make_pair(delimiter, line_number));
                }
                left = right + 1;
            } else {
                std::string remaining = str.substr(left);
                // Check the remaining part for newlines and adjust line_number accordingly
                for(size_t i = 0; i < remaining.length(); ++i) {
                    if(remaining[i] == '\n') {
                        ++line_number;
                    }
                }
                std::string content = Parser::trim(remaining, "\t\n ");
                if (!content.empty())
                    ret.push_back(std::make_pair(content, line_number));
                break;
            }
        }
        return ret;
    }

std::string	Parser::itoa(int num)
{
	std::ostringstream	oss;

	oss << num;
	return oss.str();
}