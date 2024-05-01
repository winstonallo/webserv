#ifndef UTILS_HPP
#define UTILS_HPP

#include <sys/socket.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <map>
#include <stdbool.h>

class Server;
class LocationInfo;

typedef enum e_exception
{
	THROW,
	LOG
} e_exception;

namespace Utils
{
	bool											write_access(const std::string& path);
	bool											read_access(const std::string& path);
	bool											file_exists(const std::string& path);
	std::string										trim(const std::string& str, const std::string& delimiters);
	std::string										trim_comment(const std::string& str, const std::string& delimiters);
	std::string										itoa(int num);
	std::vector <std::string>						split_keep_quoted_words(const std::string& str, const std::string& delimiters);
	std::vector <std::pair <std::string, int> > 	split_keep_delimiters(const std::string& str, const std::string& delimiters);
	std::string 									get_file_extension(const std::string& file_path);
	int												extract_numeric_value(const std::string& str);
	int												parse_client_max_body_size(const std::string& client_max_body_size);
	std::map <int, std::string>						get_status_codes();
	std::map <std::string, std::string>				get_content_types();
	std::string										pathconcat(std::string s1, std::string s2);
	std::string										generate_default_error_page(const int status_code);
	std::string										extract_location_name(const std::string& location);
	void											validate_required_server_values(Server* new_server);
	bool											is_file(const std::string& path);
	bool											is_directory(const std::string& path);
	void											notify_client_connection(Server* server, int client_socket, struct sockaddr_storage client_addr);
	std::string										get_cgi_script_name(const std::string& uri);
	std::string										to_lower(const std::string& str);
	std::string										extract_cgi_identifier(const std::string& key);
	void											config_error_on_line(int line_number, const std::string& error_message, e_exception throw_exception=LOG);
	std::string										safe_ifstream(const std::string& path);
}

#define MAX_FILE_SIZE 140000
#define UNCLOSED_QUOTE "unclosed quote sequence"
#define DEFAULT_ERROR_PAGE "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
<meta charset=\"UTF-8\">\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
<title>400</title>\n\
</head>\n\
<body>\n\
<div class=\"container bsod\">\n\
  <div class=\"neg\">\n\
    <h1 class=\"bg title\">error 400</h1>\n\
    <p>bad request</p>\n\
  </div>\n\
  <div class=\"nav\">\n\
    <a href=\"/\" class=\"link\">go home</a>\n\
    <a href=\"/contact\" class=\"link\">contact us</a>\n\
  </div>\n\
</div>\n\
</body>\n\
</html>"

#endif
