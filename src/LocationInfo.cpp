#include "LocationInfo.hpp"
#include <string>
#include <vector>

LocationInfo::LocationInfo() : _directory_listing_enabled(false)
{

}

LocationInfo::~LocationInfo()
{

}

LocationInfo::LocationInfo(const LocationInfo& rhs)
{
	*this = rhs;
}

LocationInfo&	LocationInfo::operator=(const LocationInfo& rhs)
{
	if (this != &rhs)
	{
		_path = rhs._path;
		_autoindex = rhs._autoindex;
	}
	return (*this);
}

std::string	LocationInfo::getPath() const
{
	return (_path);
}

void	LocationInfo::setPath(std::string p)
{
	_path = p;
}

bool	LocationInfo::getAutoindex() const
{
	return (_autoindex);
}

void	LocationInfo::setAutoindex(bool val)
{
	_autoindex = val;
}

std::vector <std::string>	LocationInfo::get_allowed_methods() const
{
	return _allowed_methods;
}

void	LocationInfo::set_allowed_methods(std::vector <std::string>& allowed_methods)
{
	_allowed_methods = allowed_methods;
}

bool	LocationInfo::directory_listing_enabled() const
{
	return _directory_listing_enabled;
}

void	LocationInfo::set_directory_listing(bool directory_listing_enabled)
{
	_directory_listing_enabled = directory_listing_enabled;
}

std::string	LocationInfo::get_name() const
{
	return _name;
}

void	LocationInfo::set_name(const std::string& name)
{
	_name = name;
}

std::string		LocationInfo::get_root() const
{
	return _root;
}

void	LocationInfo::set_root(const std::string& r)
{
	_root = r;
}

std::string		LocationInfo::get_return() const
{
	return _return;
}

void			LocationInfo::set_return(const std::string& r)
{
	_return = r;
}

void			LocationInfo::set_alias(const std::string& al)
{
	_alias = al;
}

std::string		LocationInfo::get_alias() const
{
	return _alias;
}

void			LocationInfo::set_cgi_path(std::vector<std::string> p)
{
	_cgi_path = p;
}

std::vector<std::string> LocationInfo::get_cgi_path() const
{
	return _cgi_path;
}

void			LocationInfo::set_cgi_extension(std::vector<std::string> e)
{
	_cgi_ext = e;
}

std::vector<std::string>	LocationInfo::get_cgi_extension() const
{
	return _cgi_ext;
}

void			LocationInfo::set_client_max_body_size(int mb)
{
	_client_max_body_size = mb;
}

int				LocationInfo::get_client_max_body_size() const
{
	return _client_max_body_size;
}