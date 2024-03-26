#ifndef CONFIGSETTER_HPP
#define CONFIGSETTER_HPP

#include "LocationInfo.hpp"
#include <vector>
#include <string>

namespace Setters
{
    void	set_root(const std::vector <std::string>& root, LocationInfo*& new_location);
    void	set_directory_listing(const std::vector <std::string>& directory_listing, LocationInfo*& new_location);
    void	set_allowed_methods(const std::vector <std::string>& allowed_methods, LocationInfo*& new_location);
    void	set_return(const std::vector <std::string>& rtrn, LocationInfo*& new_location);
    void	set_alias(const std::vector <std::string>& alias, LocationInfo*& new_location);
    void	set_cgi_path(const std::vector <std::string>& cgi_path, LocationInfo*& new_location);
    void	set_cgi_extension(const std::vector <std::string>& extension, LocationInfo*& new_location);
    void	set_autoindex(const std::vector <std::string>& autoindex, LocationInfo*& new_location);
}

#endif