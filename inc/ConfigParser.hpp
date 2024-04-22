#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <stack>

class ConfigParser
{
	public:
	
		ConfigParser(const std::string& path="config_files/webserv.conf");
		~ConfigParser() {}

		std::map <std::string, std::vector <std::string> >						get_config() const { return _config; }
		std::map <int, std::string>												get_error_pages() { return _error_pages; }
		std::map <int, std::map <std::string, std::vector <std::string> > >		get_servers() { return _servers; }

	private:

		std::string																error_on_line(const std::string& issue, int line_count);
		std::string																error(const std::string& message);

		void																	load_config_from_file(const std::string& config_path);
		void																	parse_config_from_vector(const std::vector < std::pair <std::string, int> >& config);
		void																	handle_opening_brace(const std::pair <std::string, int>& prev_line);
		void																	handle_closing_brace(const std::pair <std::string, int>& prev_line);
		void																	store_key_value_pairs(const std::pair <std::string, int> prev_line);
		void																	validate_nesting(int line_count);
		void																	validate_config_header(const std::vector <std::pair <std::string, int> >& config);
		void																	validate_config(const std::string& config);
		std::string																remove_comments(const std::string& config);

		void																	dispatch_values();
	
		std::string																_config_file_path;
		std::stack <std::string> 												_nesting_level;
		size_t																	_server_count;

		std::map <std::string, std::vector <std::string> >						_config;

		std::map <int, std::map <std::string, std::vector <std::string> > >		_servers;
		std::map <int, std::string>												_error_pages;

		ConfigParser(const ConfigParser&) {}
		ConfigParser&	operator=(const ConfigParser&) { return *this; }
};

#define EXPECTED_EXT ".conf"
#define UNINITIALIZED_SCOPE "uninitialized scope: expected identifier"             
#define UNEXPECTED_NL "unexpected newline - terminate value lines with ';'"
#define UNTERM_VALUE_SCOPE "unterminated value scope - terminate value lines with ';'"
#define EMPTY "config file is empty"
#define MISSING_CLOSING_BRACE "missing closing brace - terminate scope with '}'"
#define EXTRA_CLOSING_BRACE "extraneous closing brace"
#define MISSING_OPENING_BRACE "missing opening brace - use '{ }' for nesting"
#define INVALID_EXT "invalid file extension (expected: '.conf')"
#define INV_HEADER "invalid config file header (expected: 'webserv')"
#define NOT_FOUND "could not open file"
#define FALLBACK "falling back to default config: '\033[1mconfig/webserv.conf\033[0m'"
#define RULES "refer to '\033[1mconfig/rules/config-formatting-rules.md\033[0m' for more details"

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"

std::ostream& operator<<(std::ostream& os, const ConfigParser& config);

#endif