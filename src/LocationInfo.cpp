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
		path = rhs.path;
		autoindex = rhs.autoindex;
	}
	return (*this);
}

std::string	LocationInfo::getPath() const
{
	return (path);
}

void	LocationInfo::setPath(std::string p)
{
	path = p;
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