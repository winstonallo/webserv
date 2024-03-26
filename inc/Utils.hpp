#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <unistd.h>
#include <map>

namespace Utils 
{
	bool											write_access(const std::string& path);
	bool											file_exists(const std::string& path);
	void											get_pair(const std::string& str, std::vector <std::pair <std::string, int> >& ret, size_t left, size_t right, int& line_number);
	std::string										trim(const std::string& str, const std::string& delimiters);
	std::string										trim_comment(const std::string& str, const std::string& delimiters);
	std::string										itoa(int num);
	std::vector <std::string>						split(const std::string& str, const std::string& delimiters);
	std::vector <std::string>						split_keep_quoted_words(const std::string& str, const std::string& delimiters);
	std::vector <std::pair <std::string, int> > 	split_keep_delimiters(const std::string& str, const std::string& delimiters);
	std::string 									get_file_extension(const std::string& file_path);
	int												extract_numeric_value(const std::string& str);
	int												parse_client_max_body_size(const std::string& client_max_body_size);
	std::string										file_to_string(const std::string& path);
	std::map <int, std::string>						get_error_status_codes();
	std::string										pathconcat(std::string s1, std::string s2);
};

#define UNCLOSED_QUOTE "unclosed quote sequence"

#endif