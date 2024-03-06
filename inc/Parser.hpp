#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#pragma once

#include "Headers.hpp"

// static class for abstractable parsing utils
class Parser
{
	public:

		static std::vector <std::string> 	split(const std::string& str, const std::string& delimiters);
		static std::vector <std::string>	split_keep_delimiters(const std::string& str, const std::string& delimiters);
		static std::string					trim(const std::string& str, const std::string& delimiters);
		static std::string					trim_comment(const std::string& str, const std::string& delimiters);

	private:
		Parser();
		Parser(const Parser& rhs);
		Parser &operator=(const Parser& rhs);
};

#endif