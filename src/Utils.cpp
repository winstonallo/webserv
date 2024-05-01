#include "Utils.hpp"
#include <fstream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <limits.h>
#include "Log.hpp"
#include "Server.hpp"
#include "Config.hpp"

namespace Utils
{
	int		find_closest_valid_size(int input)
	{
		const int valid_sizes[] = {1, 2, 4, 8, 16};
		const int num_sizes = sizeof(valid_sizes) / sizeof(valid_sizes[0]);

		int closest_size = valid_sizes[0];
		int min_difference = INT_MAX;

		for (int i = 0; i < num_sizes; i++)
		{
			int difference = abs(input - valid_sizes[i]);

			if (difference < min_difference)
			{
				min_difference = difference;
				closest_size = valid_sizes[i];
			}
		}
		if (closest_size != input)
		{
			config_error_on_line(
				-1,
				"Client max body size " +
				itoa(input) +
				"M is not valid, using closest supported value: " +
				itoa(closest_size) +
				"M.",
				LOG
			);
		}
		return closest_size;
	}

	int		parse_client_max_body_size(const std::string& client_max_body_size)
	{
		size_t non_digit_char_pos = client_max_body_size.find_first_not_of("0123456789");

		if (client_max_body_size[non_digit_char_pos] != 'M' || non_digit_char_pos != client_max_body_size.size() - 1)
		{
			return -1;
		}
		else
		{
			std::string digit_str = client_max_body_size.substr(0, non_digit_char_pos).c_str();
			int size = std::atoi(digit_str.c_str());

			int valid_size = find_closest_valid_size(size);
			return valid_size * MEGABYTE;
		}
	}

	bool	file_exists(const std::string& path)
	{
		std::ifstream	file(path.c_str());

		return file.good();
	}

	std::string		trim(const std::string& str, const std::string& delimiters)
	{
		size_t	first = str.find_first_not_of(delimiters);
		if (first == std::string::npos)
			return "";
		size_t	last = str.find_last_not_of(delimiters);
		return str.substr(first, (last - first + 1));
	}

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

	void	get_pair(const std::string& str, std::vector <std::pair <std::string, int> >& ret, size_t left, size_t right, int& line_number)
	{
		std::string segment = str.substr(left, right);
		line_number += std::count(segment.begin(), segment.end(), '\n');
		if (trim(segment, "\t\n ") != "")
		{
			ret.push_back(std::make_pair(trim(segment, "\t\n "), line_number));
		}
	}

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

	std::string	itoa(int num)
	{
		std::ostringstream	oss;

		oss << num;
		return oss.str();
	}

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

	std::map <int, std::string>	get_status_codes()
	{	
		std::map <int, std::string>	error_status_codes;

		error_status_codes[100] = "Continue";
		error_status_codes[101] = "Switching Protocol";
		error_status_codes[200] = "OK";
		error_status_codes[201] = "Created";
		error_status_codes[202] = "Accepted";
		error_status_codes[203] = "Non-Authoritative Information";
		error_status_codes[204] = "No Content";
		error_status_codes[205] = "Reset Content";
		error_status_codes[206] = "Partial Content";
		error_status_codes[300] = "Multiple Choice";
		error_status_codes[301] = "Moved Permanently";
		error_status_codes[302] = "Moved Temporarily";
		error_status_codes[303] = "See Other";
		error_status_codes[304] = "Not Modified";
		error_status_codes[307] = "Temporary Redirect";
		error_status_codes[308] = "Permanent Redirect";
		error_status_codes[400] = "Bad Request";
		error_status_codes[401] = "Unauthorized";
		error_status_codes[403] = "Forbidden";
		error_status_codes[404] = "Not Found";
		error_status_codes[405] = "Method Not Allowed";
		error_status_codes[406] = "Not Acceptable";
		error_status_codes[407] = "Proxy Authentication Required";
		error_status_codes[408] = "Request Timeout";
		error_status_codes[409] = "Conflict";
		error_status_codes[410] = "Gone";
		error_status_codes[411] = "Length Required";
		error_status_codes[412] = "Precondition Failed";
		error_status_codes[413] = "Payload Too Large";
		error_status_codes[414] = "URI Too Long";
		error_status_codes[415] = "Unsupported Media Type";
		error_status_codes[416] = "Requested Range Not Satisfiable";
		error_status_codes[417] = "Expectation Failed";
		error_status_codes[418] = "I'm a teapot";
		error_status_codes[421] = "Misdirected Request";
		error_status_codes[425] = "Too Early";
		error_status_codes[426] = "Upgrade Required";
		error_status_codes[428] = "Precondition Required";
		error_status_codes[429] = "Too Many Requests";
		error_status_codes[431] = "Request Header Fields Too Large";
		error_status_codes[451] = "Unavailable for Legal Reasons";
		error_status_codes[500] = "Internal Server Error";
		error_status_codes[501] = "Not Implemented";
		error_status_codes[502] = "Bad Gateway";
		error_status_codes[503] = "Service Unavailable";
		error_status_codes[504] = "Gateway Timeout";
		error_status_codes[505] = "HTTP Version Not Supported";
		error_status_codes[506] = "Variant Also Negotiates";
		error_status_codes[507] = "Insufficient Storage";
		error_status_codes[510] = "Not Extended";
		error_status_codes[511] = "Network Authentication Required";

		return error_status_codes;
	}

	std::map <std::string, std::string> get_content_types()
	{
		std::map <std::string, std::string> content_types;

		content_types["default"] = 	"text/html";
		content_types[".html"] 	= 	"text/html";
		content_types[".htm"] 	= 	"text/html";
		content_types[".css"] 	= 	"text/css";
		content_types[".txt"] 	= 	"text/plain";
		content_types[".bmp"] 	= 	"image/bmp";
		content_types[".gif"] 	= 	"image/gif";
		content_types[".ico"] 	= 	"image/x-icon";
		content_types[".ico"] 	= 	"image/x-icon";
		content_types[".jpg"] 	= 	"image/jpeg";
		content_types[".jpeg"]	= 	"image/jpeg";
		content_types[".png"] 	= 	"image/png";
		content_types[".pdf"] 	= 	"application/pdf";
		content_types[".gz"] 	= 	"application/x-gzip";
		content_types[".doc"] 	= 	"application/msword";
		content_types[".avi"] 	= 	"video/x-msvideo";
		content_types[".mp3"] 	= 	"audio/mp3";

		return content_types;
	}

	std::string	generate_default_error_page(const int status_code)
	{
		std::string default_error_code = "400";
		std::string default_error_message = "bad request";
		std::string default_html = DEFAULT_ERROR_PAGE;
		std::string	new_error_code = Utils::itoa(status_code);
		std::string new_error_message = get_status_codes()[status_code];

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
		if (is_directory("error_pages") == false)
		{
			if (mkdir("error_pages", 0777) == -1)
			{
				Log::log("Error: Could not create error_pages directory\n", STD_ERR | ERROR_FILE);
				return DEFAULT_ERROR_PAGE;
			}
		}

		std::string new_html_path = "error_pages/" + Utils::itoa(status_code) + ".html";

		std::ofstream	oss(new_html_path.c_str());

		if (!oss)
		{
			Log::log("Error: Could not create default error page, falling back to 400: Bad Request\n", STD_ERR | ERROR_FILE);
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

	bool is_regular_file(const std::string& path)
	{
		struct stat path_stat;

		if (stat(path.c_str(), &path_stat) == -1)
		{
			throw std::runtime_error("Error: unable to stat file " + path + "\n");
		}
		return S_ISREG(path_stat.st_mode);
	}

	std::string	safe_ifstream(const std::string &path)
	{
		if (is_regular_file(path) == false)
		{
			std::string error_message = "Error: " + path + " is not a regular file.\n";
			throw std::runtime_error(error_message);
		}

		std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
		
		if (!file)
		{
			std::string error_message = "Error: " + path + " not found.\n";
			throw std::runtime_error(error_message);
		}

		std::ifstream::pos_type size = file.tellg();

		if (size > MAX_FILE_SIZE)
		{
			std::string error_message = "Error: " + path + " is too big (max. 14KB)\n";
			throw std::runtime_error(error_message);
		}

		file.seekg(0, std::ios::beg);
		std::vector <char> buffer(size);
		file.read(&buffer[0], size);

		if (file.fail() == true)
		{
			std::string error_message = "Error reading " + path + "\n";
			throw std::runtime_error(error_message);
		}

		return std::string(buffer.begin(), buffer.end());
	}
}
