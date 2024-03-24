#include "LocationInfo.hpp"
#include <ostream>
#include <string>
#include <vector>

LocationInfo::LocationInfo() : _directory_listing_enabled(false), _is_cgi(false)
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
	return (_root);
}

void	LocationInfo::set_root(const std::vector <std::string>& root)
{
	if (root.empty() == false)
	{
		_root = root[0];
	}
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

void	LocationInfo::set_allowed_methods(const std::vector <std::string>& allowed_methods)
{
	_allowed_methods = allowed_methods;
}

bool	LocationInfo::directory_listing_enabled() const
{
	return _directory_listing_enabled;
}

void	LocationInfo::set_directory_listing(const std::vector <std::string>& directory_listing_enabled)
{
	if (directory_listing_enabled.empty() == false)
	{
		if (directory_listing_enabled[0] == "enabled")
		{
			_directory_listing_enabled = true;
		}
	}
}

std::string	LocationInfo::get_name() const
{
	return _name;
}

void	LocationInfo::set_name(const std::string& name)
{
	_name = name;
}

std::string LocationInfo::get_return() const
{
	return _return;
}

void	LocationInfo::set_return(const std::vector <std::string>& r)
{
	if (r.empty() == false)
	{
		_return = r[0];
	}
}

std::string	LocationInfo::get_alias() const
{
	return _alias;
}

void	LocationInfo::set_alias(const std::vector <std::string>& a)
{
	if (a.empty() == false)
	{
		_alias = a[0];
	}
}

void	LocationInfo::set_cgi_path(const std::vector <std::string>& p)
{
	if (p.empty() == false)
	{
		_cgi_path = p[0];
	}
}

std::string	LocationInfo::get_cgi_path() const
{
	return _cgi_path;
}

void	LocationInfo::set_cgi_extension(const std::vector <std::string>& e)
{
	if (e.empty() == false)
	{
		_cgi_ext = e[0];
	}
}

std::string	LocationInfo::get_cgi_extension() const
{
	return _cgi_ext;
}

int	LocationInfo::get_client_max_body_size() const
{
	return _client_max_body_size;
}

void	LocationInfo::set_client_max_body_size(int n)
{
	_client_max_body_size = n;
}

bool	LocationInfo::is_cgi() const
{
	return _is_cgi;
}

void	LocationInfo::set_is_cgi(bool is_cgi)
{
	_is_cgi = is_cgi;
}

std::ostream& operator<<(std::ostream& os, const LocationInfo& rhs)
{
	os << "\troot: " << rhs.getPath() << std::endl;
	os << "\tname: " << rhs.get_name() << std::endl;
	if (rhs.directory_listing_enabled() == true)
	{
		os << "\tdirectory listing: enabled" << std::endl;
	}
	else
	{
		os << "\tdirectory listing: disabled" << std::endl;
	}
	os << "\tallowed methods: ";
	for (size_t i = 0; i < rhs.get_allowed_methods().size(); i++)
	{
		os << "\t" << rhs.get_allowed_methods()[i] << " ";
	}
	return os;
}