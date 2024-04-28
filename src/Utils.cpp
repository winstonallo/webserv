#include "Utils.hpp"
#include <fstream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "Log.hpp"
#include "Server.hpp"

namespace Utils
{
	int		parse_client_max_body_size(const std::string& client_max_body_size)
	{
		size_t non_digit_char_pos = client_max_body_size.find_first_not_of("0123456789");

		if (client_max_body_size[non_digit_char_pos] != 'M' || non_digit_char_pos != client_max_body_size.size() - 1)
		{
			return -1;
		}
		else
		{
			return std::atoi(client_max_body_size.substr(0, non_digit_char_pos).c_str()) * 1000000;
		}
	}

	bool	write_access(const std::string& path)
	{
	    return access(path.c_str(), W_OK) == 0;
	}

	// checks existence of file
	//
	// @param path:		path to file to check
	// @return (bool):	true if file exists, else false
	bool	file_exists(const std::string& path)
	{
		std::ifstream	file(path.c_str());

		return file.good();
	}

	// trims line based on delimiters
	//
	// @param	str: string to trim
	// @param	delimiters: string containing all delimiters
	//
	// @return	new trimmed string
	std::string		trim(const std::string& str, const std::string& delimiters)
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
	std::vector <std::string>	split(const std::string& str, const std::string& delimiters)
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
	std::vector <std::string>	split_keep_quoted_words(const std::string& str, const std::string& delimiters)
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
	std::string		trim_comment(const std::string& str, const std::string& delimiters)
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
		if (trim(segment, "\t\n ") != "")
		{
			ret.push_back(std::make_pair(trim(segment, "\t\n "), line_number));
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
	std::vector <std::pair <std::string, int> > split_keep_delimiters(const std::string& str, const std::string& delimiters)
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
	std::string	itoa(int num)
	{
		std::ostringstream	oss;

		oss << num;
		return oss.str();
	}

	//finds the extension of a file in path
	std::string get_file_extension(const std::string& file_path)
	{
		size_t pos = file_path.rfind(".");
		if (pos != std::string::npos && pos < file_path.size() - 1)
		{
			return file_path.substr(pos);
		}
		return std::string("");
	}

	int	extract_numeric_value(const std::string& str)
	{
		std::string numeric_value = str.substr(str.find_first_of("0123456789"), str.find_last_of("0123456789"));

		return std::atoi(numeric_value.c_str());
	}

	std::string	file_to_string(const std::string& path)
	{
		std::ifstream	input_file(path.c_str());

		if (input_file.is_open() == false)
		{
			throw std::runtime_error("error: could not open " + path + ": " +  strerror(errno));
		}

		std::stringstream buffer;
		
		buffer << input_file.rdbuf();

		return buffer.str();
	}

	std::map <int, std::string>	get_error_status_codes()
	{	
		std::map <int, std::string>	error_status_codes;

		error_status_codes[400] = "bad request";
		error_status_codes[401] = "unauthorized";
		error_status_codes[402] = "payment required";
		error_status_codes[403] = "forbidden";
		error_status_codes[404] = "not found";
		error_status_codes[405] = "method not allowed";
		error_status_codes[406] = "not acceptable";
		error_status_codes[407] = "proxy authentication required";
		error_status_codes[408] = "request timeout";
		error_status_codes[409] = "conflict";
		error_status_codes[410] = "gone";
		error_status_codes[411] = "length required";
		error_status_codes[412] = "precondition failed";
		error_status_codes[413] = "payload too large";
		error_status_codes[414] = "uri too long";
		error_status_codes[415] = "unsupported media type";
		error_status_codes[416] = "range not satisfiable";
		error_status_codes[417] = "expectation failed";
		error_status_codes[418] = "i'm a teapot";
		error_status_codes[421] = "misdirected request";
		error_status_codes[422] = "unprocessable entity";
		error_status_codes[423] = "locked";
		error_status_codes[424] = "failed dependency";
		error_status_codes[425] = "too early";
		error_status_codes[426] = "upgrade required";
		error_status_codes[428] = "precondition required";
		error_status_codes[429] = "too many requests";
		error_status_codes[431] = "request header fields too large";
		error_status_codes[444] = "no response";
		error_status_codes[449] = "retry with";
		error_status_codes[451] = "unavailable for legal reasons";
		error_status_codes[499] = "client closed request";
		error_status_codes[500] = "internal server error";
		error_status_codes[501] = "not implemented";
		error_status_codes[502] = "bad gateway";
		error_status_codes[503] = "service unavailable";
		error_status_codes[504] = "gateway timeout";
		error_status_codes[505] = "http version not supported";
		error_status_codes[506] = "variant also negotiates";
		error_status_codes[507] = "insufficient storage";
		error_status_codes[508] = "loop detected";
		error_status_codes[510] = "not extended";
		error_status_codes[511] = "network authentication required";

		return error_status_codes;
	}

	// returns the error page for a given status code
	//
	// if:	the status code is not found in the error pages from the config
	//		->	generate a default error page

	// generates a default error page for a given status code
	//
	// cat DEFAULT_ERROR_PAGE | sed 's/400/XXX/g' | sed 's/bad request/new message/g' > new_html_path
	std::string	generate_default_error_page(const int status_code)
	{
		std::string default_error_code = "400";
		std::string default_error_message = "bad request";
		std::string default_html = DEFAULT_ERROR_PAGE;
		std::string	new_error_code = Utils::itoa(status_code);
		std::string new_error_message = get_error_status_codes()[status_code];

		size_t pos_code = default_html.find(default_error_code);
		size_t pos_message = default_html.find(default_error_message);

		while (pos_code != std::string::npos || pos_message != std::string::npos)
		{
			if (pos_code != std::string::npos)
			{
				default_html.replace(pos_code, default_error_code.size(), new_error_code);
				pos_code = default_html.find(default_error_code, pos_code + new_error_code.size());
			}
			if (pos_message != std::string::npos)
			{
				default_html.replace(pos_message, default_error_message.size(), new_error_message);
				pos_message = default_html.find(default_error_message, pos_message + new_error_message.size());
			}
		}
		std::string new_html_path = Utils::itoa(status_code) + ".html";

		std::ofstream	oss(new_html_path.c_str());

		if (!oss)
		{
			Log::log("Error: Could not create default error page, falling back to 400: bad_request\n", STD_ERR | ERROR_FILE);
			return DEFAULT_ERROR_PAGE;
		}

		oss << default_html;
		return new_html_path;
	}

	std::map <std::string, std::string>		get_environment_map(char **env)
	{
		std::map <std::string, std::string>	env_map;
		for (int i = 0; env[i]; i++)
		{
			std::string line = std::string(env[i]);
			env_map[line.substr(0, line.find_first_of("="))] = line.substr(line.find_first_of("=") + 1);
		}
		return env_map;
	}

	std::string pathconcat(std::string s1, std::string s2)
	{
		if (s2 == "")
			return s1; 
		int l1 = s1.size();
		if (s1[l1-1] != '/' && s2[0] != '/')
			s1.append("/");
		if (s1[l1-1] == '/' && s2[0] == '/')
			s1.erase(l1-1);
		return (s1 + s2);	
	}

	std::string	extract_location_name(const std::string& current_map_key)
	{
		std::string location_key_prefix = "location";
		size_t found = current_map_key.find(location_key_prefix);

		if (found != std::string::npos) 
		{
			size_t colon = current_map_key.find(":", found);
			return current_map_key.substr(found + location_key_prefix.size() + 1, colon - (found + location_key_prefix.size()) - 1);
		}

		return "";
	}

	void	validate_required_server_values(Server* new_server)
	{
		if (new_server->get_server_name().empty() == true)
		{
			Log::log("error: server block missing server_name\n", STD_ERR | ERROR_FILE);
			throw std::runtime_error("server block missing server_name");
		}
		if (new_server->get_host_address().s_addr == 0)
		{
			Log::log("error: server block missing host address\n", STD_ERR | ERROR_FILE);
			throw std::runtime_error("server block missing host address");
		}
		if (new_server->get_port() == 0)
		{
			Log::log("error: server block missing port\n", STD_ERR | ERROR_FILE);
			throw std::runtime_error("server block missing port");
		}
	}

	bool	is_file(const std::string& path)
	{
		struct stat	buff;
		if (stat(path.c_str(), &buff) < 0)
		{
			std::stringstream ss;
			ss <<  "Error. Stat for " << path << " failed in 'is_file' function" << std::endl;
			Log::log(ss.str(), STD_ERR | ERROR_FILE);
			return false;
		}
		return (buff.st_mode & S_IFREG);
	}

	bool	is_directory(const std::string& path)
	{
		struct stat	buff;
		if (stat(path.c_str(), &buff) < 0)
		{
			Log::log("Error. Stat failed in is_directory function\n", STD_ERR | ERROR_FILE);
			return false;
		}
		return (buff.st_mode & S_IFDIR);
	}

	void	notify_client_connection(Server* server, int client_socket, struct sockaddr_storage client_addr)
	{
		std::string server_ip = inet_ntoa(server->get_host_address());
		std::string server_port = Utils::itoa(server->get_port());
		std::string socket = Utils::itoa(client_socket);
		std::string server_name = server->get_server_name()[0];
		std::string	client_ip_str = inet_ntoa(((struct sockaddr_in*)&client_addr)->sin_addr);
		std::string	client_port = Utils::itoa(ntohs(((struct sockaddr_in*)&client_addr)->sin_port));

		Log::log("Accept Connection: " + client_ip_str +
			":" + client_port + ": " +
			socket + "->" + server_name + "\n", STD_OUT | ACCEPT_FILE);
	}

	std::string	get_cgi_script_name(const std::string &uri)
	{
		size_t start_pos = uri.find("cgi-bin");
		size_t end_pos = uri.find_first_of("/", start_pos + sizeof("cgi-bin/"));

		return uri.substr(start_pos, end_pos);
	}



	std::string	to_lower(const std::string& str)
	{
		std::string res = str;
		for (size_t i = 0; i < str.length(); i++)
		{
			res[i] = std::tolower(res[i]);	
		}
		return res;
	}

	void	print_server_info(std::ostream& out, Server* server)
	{
		out << "host: " << inet_ntoa(server->get_host_address()) << std::endl;
		out << "port: " << server->get_port() << std::endl;
		out << "server_name: " << server->get_server_name()[0] << std::endl;
		out << "root: " << server->get_root() << std::endl;
		out << "index: " << server->get_index_path() << std::endl;
		out << "autoindex: " << server->get_auto_index() << std::endl;

		std::vector <std::string> server_name = server->get_server_name();
		for (std::vector <std::string>::iterator it = server_name.begin(); it != server_name.end(); it++) { out << *it << " "; }
		out << std::endl;

		out << "access_log: " << server->get_access_log() << std::endl;
		out << "client_max_body_size: " << server->get_client_max_body_size() << std::endl;
		out << "locations: " << std::endl;
	}

	void	print_location_info(std::ostream& out, LocationInfo* location)
	{
		out << "root: " << location->get_path() << std::endl;
		out << "\tdirectory_listing: " << (location->get_directory_listing() ? "enabled" : "disabled") << std::endl;
		out << "\tautoindex: " << (location->get_autoindex() ? "enabled" : "disabled") << std::endl;
		out << "\tallowed_methods: ";

		std::vector <std::string> allowed_methods = location->get_allowed_methods();
		for (std::vector <std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); it++) { out << *it << " "; }
		out << std::endl;

		if (location->get_cgi() == true)
		{
			out << "\tcgi_handler: " << location->get_cgi_handler() << std::endl;
			out << "\tcgi_extension: " << location->get_cgi_extensions()[0] << std::endl;
		}

		out << std::endl;
	}

	std::string	extract_cgi_identifier(const std::string& key)
	{
		if (std::count(key.begin(), key.end(), ':') < 2)
		{
			return "";
		}
		std::string cgi_prefix = "location /cgi-bin";
		std::string cgi_identifier = key.substr(cgi_prefix.size() + 1);
		cgi_identifier =  "/" + cgi_identifier.substr(0, cgi_identifier.find_first_of(":"));
		return cgi_identifier;
	}

	void config_error_on_line(int line_number, const std::string& error_message, e_exception throw_exception)
	{
		if (line_number != -1)
		{
			Log::log("Config file (line " + Utils::itoa(line_number) + "): Error: " + error_message + "\n");
		}

		else
		{
			Log::log("Config file: Error: " + error_message + "\n");
		}

		if (throw_exception == THROW)
		{
			throw std::runtime_error("");
		}
	}
}
