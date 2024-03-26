#include "ConfigSetters.hpp"
#include "LocationInfo.hpp"
#include <vector>
#include <string>

namespace Setters 
{
    void	set_root(const std::vector <std::string>& root, LocationInfo*& new_location)
    {
        if (root.empty() == false)
        {
            new_location->set_root(root[0]);
        }
    }

    void	set_directory_listing(const std::vector <std::string>& directory_listing, LocationInfo*& new_location)
    {
        if (directory_listing.empty() == false)
        {
            if (directory_listing[0] == "enabled")
            {
                new_location->set_directory_listing(true);
            }
        }
    }

    void	set_allowed_methods(const std::vector <std::string>& allowed_methods, LocationInfo*& new_location)
    {
        if (allowed_methods.empty() == false)
        {
            new_location->set_allowed_methods(allowed_methods);
        }
        else 
        {
            std::vector <std::string> foo;
            foo.push_back("none");
            new_location->set_allowed_methods(allowed_methods);
        }
    }

    void	set_return(const std::vector <std::string>& rtrn, LocationInfo*& new_location)
    {
        if (rtrn.empty() == false)
        {
            new_location->set_return(rtrn[0]);
        }
    }

    void	set_alias(const std::vector <std::string>& alias, LocationInfo*& new_location)
    {
        if (alias.empty() == false)
        {
            new_location->set_alias(alias);
        }
    }

    void	set_cgi_path(const std::vector <std::string>& cgi_path, LocationInfo*& new_location)
    {
        if (cgi_path.empty() == false)
        {
            new_location->set_cgi_path(cgi_path[0]);
        }
    }


    void	set_cgi_extension(const std::vector <std::string>& extension, LocationInfo*& new_location)
    {
        if (extension.empty() == false)
        {
            new_location->set_cgi_extensions(extension);
        }
    }

    void	set_autoindex(const std::vector <std::string>& autoindex, LocationInfo*& new_location)
    {
        if (autoindex.empty() == false)
        {
            if (autoindex[0] == "enabled")
            {
                new_location->set_autoindex(true);
            }
        }
    }
}