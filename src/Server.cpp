#include "Server.hpp"
#include "LocationInfo.hpp"
#include <netinet/in.h>
#include <string>
#include <vector>
#include "Director.hpp"
#include <fstream>
#include "Log.hpp"

Server::Server() 
{
	init_status_strings();
	init_content_types(); 
}

Server::~Server()
{
	std::vector<LocationInfo*>::iterator it;
	for (it = locations.begin(); it != locations.end(); it++)
	{
		delete *it;
	}
}


Server::Server(int tfd, struct sockaddr_storage ss, size_t taddr_len):
	Node(tfd, ss, taddr_len, SERVER_NODE)
{
	init_status_strings();
	init_content_types(); 
}

Server::Server(const Server& rhs) : Node()
{*this = rhs;}

Server&	Server::operator=(const Server& rhs)
{
	if (this != &rhs)
	{
		port = rhs.port;
		server_name = rhs.server_name;
//		address = rhs.address;
		autoindex = rhs.autoindex;
		root = rhs.root;
		error_log = rhs.error_log;
		access_log = rhs.access_log;
		locations = rhs.locations;
	}
	return (*this);
}

int	Server::get_port() const
{
	return (port);
}

void	Server::set_port(int prt)
{
	port = prt;
}

void	Server::set_server_name(const std::vector <std::string>& tserver_name)
{
	server_name = tserver_name;
}

std::vector <std::string>	Server::get_server_name() const
{
	return server_name;
}

bool	Server::get_auto_index() const
{
	return (autoindex); 
}

void	Server::set_auto_index(bool t)
{
	autoindex = t;
}

std::string	Server::get_error_log() const
{
	return (error_log);
}

void	Server::set_error_log(const std::string& log)
{
	error_log = log;
}

std::string	Server::get_access_log() const
{
	return (access_log);
}

void	Server::set_access_log(const std::string& log)
{
	access_log = log;
}

struct in_addr		Server::get_host_address() const
{
	return _host_address;
}

void	Server::set_host_address(struct in_addr& host)
{
	_host_address = host;
}

int	Server::get_client_max_body_size() const
{
	return _client_max_body_size;
}

void	Server::set_client_max_body_size(const int client_max_body_size)
{
	_client_max_body_size= client_max_body_size;
}

void	Server::add_locations(std::vector <LocationInfo*> locations)
{
	this->locations = locations;
}

std::vector <LocationInfo*>	Server::get_locations() const
{
	return locations;
}

std::string Server::respond(Request& rq)
{
	if (rq.get_method() == "GET" || rq.get_method() == "HEAD")
	{
		std::ifstream file("index.html");
		if (file.fail())
		{
			return "Error"; 
		}
		std::ostringstream ss;
		ss << file.rdbuf();
		return ss.str();
	}
	return "Error";
}

//Server

int		Server::get_error_code() const
{
	return errcode;
}

void	Server::set_error_code(int cd)
{
	errcode = cd;
}

void	Server::set_director(Director *d)
{
	director = d;
}

Director*	Server::get_director() const
{
	return director;
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

std::string		Server::create_response(Request& rq)
{
	std::stringstream 	ss;
	std::string 		ex;
	char				buf[100];
	std::string			body;
	bool				failed = false;

	if (rq.get_errcode() == 0)
	{
		try
		{
			body = get_body(rq);
		}
		catch(const std::exception& e)
		{
			failed = true;
		}
	}
	else
		failed = true;
	
	if (failed)
	{
		std::ifstream error_file(director->get_config()->get_error_page(errcode).c_str());
		if (error_file.fail())
		{
			Log::log("Error reading error page file.\n", STD_ERR | ERROR_FILE);
			body = DEFAULT_ERROR_PAGE;
		}
		std::stringstream ss;
		ss << error_file.rdbuf();
		body = ss.str();
	}

	ss << "HTTP/1.1 " << errcode << " " << status_string[errcode]  << "\r\n";

	time_t	curr_time = time(NULL);
	struct tm tim = *gmtime(&curr_time);
	strftime(buf, sizeof(buf), "%a, %d, %b %Y %H:%M:%S %Z", &tim);
	ss << "Date: " << buf << "\r\n";

	ss << "Server: Awesome SAD Server/1.0" << "\r\n";

	ss << "Content Length: " << body.length() << "\r\n";

	ex = Utils::get_file_extension(rq.get_path()); 
	if (errcode != 200 || ex == "")
		ex = "default";
	ss << "Content-Type: " << content_type[ex] << "\r\n";

	ss << "Connection: " << rq.get_header("CONNECTION") << "\r\n";
	ss << "\r\n";
	ss << body;
	response = ss.str();
	return response;
}

std::string		Server::get_body(Request& rq)
{
	if (rq.get_method() == "GET" || rq.get_method() == "HEAD")
	{

		std::ifstream file(rq.get_uri().c_str());
		std::cout << rq.get_uri() << std::endl;
		if (file.fail())
		{
			errcode = 400;
			Log::log("Error reading request file", STD_ERR | ERROR_FILE); 
			throw std::runtime_error("error");
		}
		std::ostringstream ss;
		ss << file.rdbuf();
		return ss.str();
	}
	return "No get";
}

int		Server::process(Request& rq)
{
	std::string location;
	LocationInfo loc_info;	
	get_best_location_match(locations, rq, location, &loc_info);
	if (!location.empty())
	{
		// is body smaller than max?
		if ((int)rq.get_body().size() > loc_info.get_client_max_body_size())
		{
			Log::log("Error. Client body is too big.\n", STD_ERR | ERROR_FILE);
			return (errcode = 413);
		}
		// is method allowed?
		std::vector<std::string> vec = loc_info.get_allowed_methods();
		std::vector<std::string>::iterator end = vec.end();
		std::vector<std::string>::iterator begin = vec.begin();
		if(std::find(begin,	end, rq.get_method()) != end)
		{
			Log::log("Error. Method not allowed.\n", STD_ERR | ERROR_FILE);
			return (errcode = 405);
		}
		// return handler
		if (loc_info.get_return().empty() == 0)
		{
			location = loc_info.get_return();
			errcode = 301;
			return (errcode);
		}
	}
	return 0;
}

void	Server::get_best_location_match(std::vector<LocationInfo*> locs, 
										Request &rq,
										std::string& best_match,
										LocationInfo* locinfo)
{
	(void)locinfo;
	int max_len = 0;
	std::vector<LocationInfo*>::iterator e = locs.end();
	std::vector<LocationInfo*>::iterator it;
	for (it = locs.begin(); it != e; it++)
	{
		if(rq.get_path().find((*it)->get_path()) == 0)
		{
			if ((*it)->get_path() == "/" ||  
			(*it)->get_path().size() == rq.get_path().size()|| 
			rq.get_path()[(*it)->get_path().size()] == '/')
			{
				if ((int)(*it)->get_path().size() > max_len)
				{
					locinfo = *it;
					best_match = (*it)->get_path();
					max_len = (*it)->get_path().size();
				}
			}
		}
	}
}
