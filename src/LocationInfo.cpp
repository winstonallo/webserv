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
		_root = rhs._root;
		_index = rhs._index;
		_autoindex = rhs._autoindex;
		_return = rhs._return;
		_alias = rhs._alias;
		_cgi_path = rhs._cgi_path;
		_cgi_ext = rhs._cgi_ext;
		_allowed_methods = rhs._allowed_methods;
		_directory_listing_enabled = rhs._directory_listing_enabled;
		_client_max_body_size = rhs._client_max_body_size;
	}
	return (*this);
}

std::string	LocationInfo::get_path() const
{
	return (_path);
}

void	LocationInfo::set_path(const std::string& p)
{
	_path = p;
}

bool	LocationInfo::get_autoindex() const
{
	return (_autoindex);
}

void	LocationInfo::set_autoindex(bool val)
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

bool	LocationInfo::get_directory_listing() const
{
	return _directory_listing_enabled;
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

void			LocationInfo::set_index_path(const std::string& loc)
{
	_index = loc;
}

std::string		LocationInfo::get_index_path() const
{
	return _index;
}