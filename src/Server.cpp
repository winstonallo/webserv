#include "Server.hpp"
#include <cstring>
#include <cerrno>
#include "LocationInfo.hpp"
#include <exception>
#include <netinet/in.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Director.hpp"
#include <fstream>
#include "Log.hpp"
#include "Utils.hpp"

Server::Server() 
{
	_status_string = Utils::get_status_codes();
	_content_type = Utils::get_content_types();
	_autoindex = false;
	_errcode = 0;
	_autoindex = false;
	_index = "";
	_root = "";
	_port = 0;
	_reloc = "";
	_client_max_body_size = 4098;
	_server_name = std::vector<std::string>();
	memset(&_host_address, 0, sizeof(_host_address));
	_error_log = "";
	_access_log = "";
	_error_pages = std::map<int, std::string>();
	_locations = std::vector<LocationInfo *>();
	_autoindex = false;
}

Server::~Server()
{
	std::vector<LocationInfo*>::iterator it;

	for (it = _locations.begin(); it != _locations.end(); it++)
	{
		delete *it;
	}
}

Server::Server(int tfd, struct sockaddr_storage ss, size_t taddr_len):
	Node(tfd, ss, taddr_len, SERVER_NODE)
{
	_status_string = Utils::get_status_codes();
	_content_type = Utils::get_content_types();
	_autoindex = false;
	_errcode = 0;
	_autoindex = false;
	_index = "";
	_root = "";
	_port = 0;
	_reloc = "";
	_client_max_body_size = 4098;
	_server_name = std::vector<std::string>();
	memset(&_host_address, 0, sizeof(_host_address));
	_error_log = "";
	_access_log = "";
	_error_pages = std::map<int, std::string>();
	_locations = std::vector<LocationInfo *>();
	_autoindex = false;
}

std::string Server::respond(Request& rq)
{
    if (rq.get_method() == "GET" || rq.get_method() == "HEAD")
    {
		try
		{
			std::string file = Utils::safe_ifstream("index.html");
			return file;
		}
		catch (const std::exception& e)
		{
			Log::log(e.what(), STD_ERR | ERROR_FILE);
			return "Error";
		}
    }
    return "Error";
}

void	Server::create_response(Request& rq, ClientInfo* client_info)
{
	std::stringstream 	ss;
	std::string 		ex;
	char				buf[100];
	std::string			body;
	bool				failed = true;

	if ((_errcode = rq.get_errcode()) == 0)
	{
		try
		{
			body = _get_body(rq, client_info);

			if (client_info->is_cgi())
			{
				return ;
			}

			failed = false;
		}
		catch(const std::exception& e)
		{
			if (!_reloc.empty())
			{
				failed = false;
				_errcode = 301;
			}
		}
	}
	if (failed)
	{
		try
		{
			std::string error_page_path = _director->get_config()->get_error_page(_errcode);
			body = Utils::safe_ifstream(error_page_path);
		}
		catch (const std::exception& e)
		{
			Log::log(e.what(), STD_ERR | ERROR_FILE);
			body = DEFAULT_ERROR_PAGE;
		}
	}
	ss << "HTTP/1.1 " << _errcode << " " << _status_string[_errcode]  << "\r\n";
	time_t	curr_time = time(NULL);
	struct tm tim = *gmtime(&curr_time);
	strftime(buf, sizeof(buf), "%a, %d, %b %Y %H:%M:%S %Z", &tim);
	ss << "Date: " << buf << "\r\n";
	ss << "Server: Awesome SAD Server/1.0" << "\r\n";
	ss << "Content-Length: " << body.length() << "\r\n";
	if (!_reloc.empty())
	{
		ss << "Location: " << _reloc << "\r\n";
	}
	ex = Utils::get_file_extension(rq.get_path()); 
	if (_errcode != 200 || ex == "")
		ex = "default";
	ss << "Content-Type: " << _content_type[ex] << "\r\n";
	if (rq.get_header("CONNECTION").empty())
	{
		ss << "Connection: " << rq.get_header("CONNECTION") << "\r\n";
	}
	ss << "\r\n";
	if (!body.empty())
		ss << body;
	client_info->set_response(ss.str());
}

std::string	Server::_do_get(std::string& location_path)
{
	std::string listing_body;

	if (_autoindex == true)
	{
		if (_get_directory_list(location_path, listing_body) < 0)
		{
			_errcode = 404;
			Log::log("Error: couldn't create directory listing", STD_ERR | ERROR_FILE);
			throw std::runtime_error("Error");
		}
		_autoindex = false;
		_errcode = 200;
		return listing_body;
	}

	try
	{
		std::string file = Utils::safe_ifstream(location_path);
		_errcode = 200;
		return file;
	}
	catch (const std::exception& e)
	{
		Log::log(e.what(), STD_ERR | ERROR_FILE);
		_errcode = 404;
		throw std::runtime_error(e.what());
	}
}

std::string		Server::_get_body(Request& rq, ClientInfo *ci)
{
	std::string	loc_path;

	_errcode = _process(rq, ci, loc_path);
	if (_errcode != 0)
	{
		throw std::runtime_error("Error");
	}
	if (ci->is_cgi())
	{
		return "";
	}
	if (rq.get_method() == "GET" || rq.get_method() == "HEAD")
	{
		return _do_get(loc_path);
	}
	else if (rq.get_method() == "PUT" || rq.get_method() == "POST")
	{
		if ((Utils::file_exists(loc_path)) && rq.get_method() == "POST")
		{
			_errcode = 204;
			throw std::runtime_error("Error");
		}
        std::string upload_dir = loc_path.substr(0, loc_path.find_last_of("/") + 1);
        if (Utils::file_exists(upload_dir) == false)
        {
            if (mkdir(upload_dir.c_str(), 0777) == -1)
            {
                _errcode = 500;
                Log::log("Error. Couldn't create directory for file upload.\n", STD_ERR | ERROR_FILE);
                throw std::runtime_error("Error");
            }
        }
		std::string filename = rq.get_header("CONTENT-DISPOSITION");
		if (filename != "default")
		{
			filename = upload_dir + filename.substr(filename.find("filename=") + 10, filename.find_last_of("\"") - filename.find("filename=") - 10);
		}
		else 
		{
			filename = loc_path;
		}
		std::ofstream file(filename.c_str(), std::ios::binary);
		if (file.fail())
		{
			_errcode = 404;
			Log::log("Error. Couldn't open location path.\n", STD_ERR | ERROR_FILE);
			throw std::runtime_error("error");
		}
		_errcode = 200;
		file.write(rq.get_body().c_str(), rq.get_body().size());
		Log::log(filename + " uploaded successfully.\n", STD_OUT);
	}
	else if (rq.get_method() == "DELETE")
	{
        std::string filename = rq.get_uri().substr(rq.get_uri().find_last_of("=") + 1);
        filename = loc_path.substr(0, loc_path.find_last_of("/") + 1) + filename;
		if (Utils::file_exists(filename) == false)
		{
			_errcode = 404;
			Log::log("Error. " + filename + " not found.\n", STD_ERR | ERROR_FILE);
			throw std::runtime_error("error");
		}
		if (remove(filename.c_str()) != 0)
		{
			_errcode = 500;
			Log::log("Error. Could not remove " + filename + ".\n", STD_ERR | ERROR_FILE);
			throw std::runtime_error("error");
		}
		Log::log(filename + " deleted successfully.\n", STD_OUT);
		_errcode = 200;
	}
	return "";
}

int		Server::_process(Request& rq, ClientInfo* ci, std::string& ret_file)
{
	LocationInfo loc_info;
	std::string loc_path;

	_get_best_location_match(_locations, rq, loc_path, &loc_info);
	if (!loc_path.empty())
	{
		if (loc_info.get_client_max_body_size() == 0)
		{
			loc_info.set_client_max_body_size(_client_max_body_size);
		}
		if ((int)rq.get_body().size() > loc_info.get_client_max_body_size())
		{
			Log::log("Error. Client body is too big.\n", STD_ERR | ERROR_FILE);
			return (_errcode = 413);
		}
		// is method allowed?
		std::vector<std::string> vec = loc_info.get_allowed_methods();
		std::vector<std::string>::iterator end = vec.end();
		std::vector<std::string>::iterator begin = vec.begin();
		if(std::find(begin,	end, rq.get_method()) == end)
		{	
			std::stringstream ss;
			ss << "Error. Method \"" << rq.get_method() << "\" not allowed.\n";
			Log::log(ss.str(), STD_ERR | ERROR_FILE);
			return (_errcode = 405);
		}
		// return handler
		if (loc_info.get_return().empty() == false)
		{
			loc_path = loc_info.get_return();
			return (_errcode = 301);
		}
		// handle cgi
		if (loc_info.get_path().find("cgi-bin") != std::string::npos)
		{
			std::string script_file_path;
			script_file_path = rq.get_path();
			script_file_path.erase(0, 1);
			if (!loc_info.get_root().empty())
				script_file_path = loc_info.get_root() + script_file_path;
			else if (!ci->get_server()->get_root().empty())
				script_file_path = ci->get_server()->get_root() + script_file_path;
			if (script_file_path == "cgi-bin")
				script_file_path.append("/" + loc_info.get_index_path()); 
			else if (script_file_path == "cgi-bin/")
				script_file_path.append(loc_info.get_index_path());
				
			std::string script_ext = Utils::get_file_extension(script_file_path);
			if (script_ext != ".sh" && script_ext != ".py")
				return (_errcode = 501);	
			if (!Utils::is_file(script_file_path))
				return (_errcode = 404);
			if (access(script_file_path.c_str(), X_OK) == -1 || access(script_file_path.c_str(), X_OK | R_OK) == -1)
				return (_errcode = 403);
			if (ci->get_cgi() == NULL)
			{
				ci->set_cgi(new CGI());
			}
			else
			{
				ci->get_cgi()->clear();
			}
			ci->set_is_cgi(true);
			ci->get_cgi()->set_path(script_file_path);
			ci->get_cgi()->initialize_environment_map(rq);
			try
			{
				ci->set_pid(ci->get_cgi()->execute(_locations, script_file_path));
			}
			catch(const std::exception& e)
			{
				Log::log("Error. CGI execution failed.\n", STD_ERR | ERROR_FILE);
				return (_errcode = 500);
			}
			return 0;
		}
		// handle alias || create loc_path path
		if (loc_info.get_alias().empty() == false) 
			ret_file = Utils::pathconcat(loc_info.get_alias(), rq.get_path().substr(loc_info.get_path().size()));
		else
		{
			if (loc_info.get_root().empty())
				ret_file = Utils::pathconcat(get_root(), rq.get_path());
			else
				ret_file = Utils::pathconcat(loc_info.get_root(), rq.get_path());
		} 

		// handle if loc_path is a directory 
		struct stat fst;
		if (stat(ret_file.c_str(), &fst) != 0)
		{
			//_errcode = 400;
			_errcode = 0;
			std::stringstream ss;
			ss << "Stat function for: " << ret_file << " failed. " << strerror(errno) << "\n";
			Log::log(ss.str(), STD_ERR | ERROR_FILE);
			return (_errcode);
		}
		if (S_ISDIR(fst.st_mode))
		{
			if (ret_file[ret_file.size() - 1] != '/')
			{
				_reloc = rq.get_path() + "/";
				return (_errcode = 301);
			}
			if (!loc_info.get_index_path().empty())
				ret_file += loc_info.get_index_path();
			else
				ret_file += get_index_path();
			if (Utils::file_exists(ret_file) == false)
			{
				if (loc_info.get_autoindex())
				{
					ret_file.erase(ret_file.find_last_of('/') + 1);
					_autoindex = true;
					return 0;
				}
				else 
					return (_errcode = 403);
			}
		}
	}
	else
	{
		loc_path = Utils::pathconcat(get_root(), rq.get_path());
		struct stat fst;
		if (stat(loc_path.c_str(), &fst) != 0)
		{
			_errcode = 400;
			Log::log("Stat function on failed.\n", STD_ERR | ERROR_FILE);
			return (_errcode);
		}
		if (S_ISDIR(fst.st_mode))
		{
			if (ret_file[ret_file.size() - 1] != '/')
			{
				_errcode = 301;
			}
		}
	}
	return 0;
}

void	Server::_get_best_location_match(std::vector<LocationInfo*> locs, 
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
					*locinfo = *(*it);
					best_match = (*it)->get_path();
					max_len = (*it)->get_path().size();
				}
			}
		}
	}
}

int	Server::_get_directory_list(std::string &path, std::string& body)
{
	DIR	*dir;
	std::stringstream ss;	
	std::string f_path;
	struct stat fst;
	struct dirent *dir_entry;
	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		Log::log("Error oppening directory for directory listing.\n", STD_ERR | ERROR_FILE);
		return -1;
	}
	ss << "<html>\n";
	ss << "<head>\n<title> Index of " << path <<"</title>\n</head>\n";
	ss << "<meta charset=\"UTF-8\">";
	ss << "<body>\n";
	ss << "<h1> Index of " + path + "</h1>\n";
	ss << "<table style=\"width:80%; font-size:15px\">\n";
	ss << "<hr>\n";
	ss << "<th style=\"text-align:left\"> Name </th>\n";
	ss << "<th style=\"text-align:left\"> Last Modified </th>\n";
	ss << "<th style=\"text-align:left\"> Size </th>\n";
	while((dir_entry = readdir(dir)) != NULL)
	{
		if (strcmp(dir_entry->d_name, ".") == 0)
			continue;
		f_path = path + dir_entry->d_name;
		stat(f_path.c_str(), &fst);
		ss << "<tr>\n<td>\n";
		if (S_ISDIR(fst.st_mode))
            ss << "&#x1F4C1;";
		else
			ss << "&#x1F4C4;";
		ss << "<a href=\"" << dir_entry->d_name;
		if (S_ISDIR(fst.st_mode))
            ss << "/";
        ss << "\">";
        ss << dir_entry->d_name;
        if (S_ISDIR(fst.st_mode))
            ss << "/";
        ss << "</a>\n</td>\n";
        ss << "<td>\n" << ctime(&fst.st_mtime) << "</td>\n";
        ss << "<td>\n";
        if (!S_ISDIR(fst.st_mode))
            ss << Utils::itoa(fst.st_size);
        ss << "</td>\n";
		ss << "</tr>\n";
    }
    ss << "</table>\n";
    ss << "<hr>\n";
    ss << "</body>\n";
    ss << "</html>\n";
    body = ss.str();
	closedir(dir);
    return (0);
}

void	Server::reset()
{
	_autoindex = false;
	_errcode = 0;
	_reloc.clear();
	_autoindex = false;
}
