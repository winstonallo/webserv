#include "LocationInfo.hpp"
#include <ostream>
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
		_root = rhs._root;
		autoindex = rhs.autoindex;
	}
	return (*this);
}

std::string	LocationInfo::get_root() const
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
	return (autoindex);
}

void	LocationInfo::setAutoindex(bool val)
{
	autoindex = val;
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

std::ostream& operator<<(std::ostream& os, const LocationInfo& rhs)
{
	os << "\troot: " << rhs.get_root() << std::endl;
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