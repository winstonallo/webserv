#include "Server.hpp"

Server::Server()
{
	init_status_strings();
	init_content_types(); 
}

Server::~Server()
{}

Server&	operator=(const Server& rhs)
{
	if (this != &rhs)
	{

	}
	return (*this);
}

Server::Server(const Server& rhs)
{
	*this = rhs;
}

Request	Server::get_request() const
{
	return request;
}

ServerInfo	Server::get_server_info() const
{
	return server_info;
}

void	Server::respond(Request rq)
{
	if (rq.get_errcode())
	{

	}
}

int		Server::get_error_code() const
{
	return errcode;
}

void	Server::set_error_code(int cd)
{
	errcode = cd;
}

void	Server::init_status_strings()
{
	status_string[100] = "Continue";
	status_string[101] = "Switching Protocol";
	status_string[200] = "OK";
	status_string[201] = "Created";
	status_string[202] = "Accepted";
	status_string[203] = "Non-Authoritative Information";
	status_string[204] = "No Content";
	status_string[205] = "Reset Content";
	status_string[206] = "Partial Content";
	status_string[300] = "Multiple Choice";
	status_string[301] = "Moved Permanently";
	status_string[302] = "Moved Temporarily";
	status_string[303] = "See Other";
	status_string[304] = "Not Modified";
	status_string[307] = "Temporary Redirect";
	status_string[308] = "Permanent Redirect";
	status_string[400] = "Bad Request";
	status_string[401] = "Unauthorized";
	status_string[403] = "Forbidden";
	status_string[404] = "Not Found";
	status_string[405] = "Method Not Allowed";
	status_string[406] = "Not Acceptable";
	status_string[407] = "Proxy Authentication Required";
	status_string[408] = "Request Timeout";
	status_string[409] = "Conflict";
	status_string[410] = "Gone";
	status_string[411] = "Length Required";
	status_string[412] = "Precondition Failed";
	status_string[413] = "Payload Too Large";
	status_string[414] = "URI Too Long";
	status_string[415] = "Unsupported Media Type";
	status_string[416] = "Requested Range Not Satisfiable";
	status_string[417] = "Expectation Failed";
	status_string[418] = "I'm a teapot";
	status_string[421] = "Misdirected Request";
	status_string[425] = "Too Early";
	status_string[426] = "Upgrade Required";
	status_string[428] = "Precondition Required";
	status_string[429] = "Too Many Requests";
	status_string[431] = "Request Header Fields Too Large";
	status_string[451] = "Unavailable for Legal Reasons";
	status_string[500] = "Internal Server Error";
	status_string[501] = "Not Implemented";
	status_string[502] = "Bad Gateway";
	status_string[503] = "Service Unavailable";
	status_string[504] = "Gateway Timeout";
	status_string[505] = "HTTP Version Not Supported";
	status_string[506] = "Variant Also Negotiates";
	status_string[507] = "Insufficient Storage";
	status_string[510] = "Not Extended";
	status_string[511] = "Network Authentication Required";
}

void	Server::init_content_types()
{
    content_type["default"] = 	"text/html";
    content_type[".html"] 	= 	"text/html";
    content_type[".htm"] 	= 	"text/html";
    content_type[".css"] 	= 	"text/css";
    content_type[".txt"] 	= 	"text/plain";
    content_type[".bmp"] 	= 	"image/bmp";
    content_type[".gif"] 	= 	"image/gif";
    content_type[".ico"] 	= 	"image/x-icon";
    content_type[".ico"] 	= 	"image/x-icon";
    content_type[".jpg"] 	= 	"image/jpeg";
    content_type[".jpeg"]	= 	"image/jpeg";
    content_type[".png"] 	= 	"image/png";
    content_type[".pdf"] 	= 	"application/pdf";
    content_type[".gz"] 	= 	"application/x-gzip";
    content_type[".doc"] 	= 	"application/msword";
    content_type[".avi"] 	= 	"video/x-msvideo";
    content_type[".mp3"] 	= 	"audio/mp3";
}

void	Server::create_response()
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << errcode << " " << status_string[errcode]  << "\r\n";
	ss << "Content-Type: " << 
	response
}