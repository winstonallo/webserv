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

        typedef std::map <std::string, std::pair <std::vector <std::string>, int> > line_count_map;

		ConfigParser(const std::string& path="assets/config/simple.conf");
		~ConfigParser() {}

		std::map <std::string, std::pair <std::vector <std::string>, int> >		get_config() const { return _config; }
		std::map <int, line_count_map>											get_servers() { return _servers; }

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

		void																	store_server_values_in_map(const std::string& key);
		void																	dispatch_servers();

	
		std::string																_config_file_path;
		std::stack <std::string> 												_nesting_level;
		size_t																	_server_count;

		std::map <std::string, std::pair <std::vector <std::string>, int> >		_config;

		line_count_map															_raw_config;
		std::map <int, line_count_map>			                                _servers;

		ConfigParser(const ConfigParser&) {}
		ConfigParser&	operator=(const ConfigParser&) { return *this; }
};

#define SERVER_PREFIX "webserv:server"
#define EXPECTED_EXT ".conf"
#define UNINITIALIZED_SCOPE "Uninitialized scope: Expected identifier."             
#define UNEXPECTED_NL "Unexpected newline - Terminate value lines with ';'."
#define UNTERM_VALUE_SCOPE "Unterminated value scope - Terminate value lines with ';'."
#define EMPTY "Configuration file is empty."
#define MISSING_CLOSING_BRACE "Missing closing brace - Terminate scope with '}'."
#define EXTRA_CLOSING_BRACE "Extraneous closing brace."
#define MISSING_OPENING_BRACE "Missing opening brace - use '{ }' for nesting."
#define INVALID_EXT "Invalid file extension (expected: '.conf')."
#define INV_HEADER "Invalid configuration file header (expected: 'webserv')."
#define NOT_FOUND "Could not open file."
#define INVALID_SCOPE "Global values are not supported."

#endif