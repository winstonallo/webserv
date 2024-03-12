#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#pragma once

#include "Headers.hpp"

// static class for abstractable parsing utils
class Parser
{
	public:

		static std::string										itoa(int n);
		static std::vector <std::string> 						split(const std::string& str, const std::string& delimiters);
		static std::vector < std::pair <std::string, int> >		split_keep_delimiters(const std::string& str, const std::string& delimiters);
		static std::vector <std::string>						split_keep_quoted_words(const std::string& str, const std::string& delimiters);
		static std::string										trim(const std::string& str, const std::string& delimiters);
		static std::string										trim_comment(const std::string& str, const std::string& delimiters);

		static std::string										error_on_line(const std::string& issue, int line_count);

	private:
		Parser();
		Parser(const Parser& rhs);
		Parser &operator=(const Parser& rhs);
};

#endif