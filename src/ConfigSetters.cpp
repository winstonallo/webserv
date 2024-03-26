#include "ConfigSetters.hpp"
#include "Log.hpp"
#include "Config.hpp"

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

    void	set_index(const std::vector <std::string>& index, LocationInfo*& new_location)
    {
        if (index.empty() == false)
        {
            new_location->set_index_path(index[0]);
        }
    }
    // validates access log from config
    //
    // 	if:		the file does not exist (meaning we can just create it)
    // 	or:		it exists and we have write access to it
    //			->	config valid, return the filename
    //
    // 	else:
    //			->	path invalid, log error & return default 
    void	configure_access_log(const std::vector <std::string>& access_log_vector, Server*& new_server)
    {
        std::string access_log;

        if (access_log_vector.empty() == false)
        {
            access_log = access_log_vector[0];

            if (Utils::file_exists(access_log) == false or Utils::write_access(access_log) == true)
            {
                new_server->set_access_log(access_log);
                return ;
            }
        }
        Log::log("error: access log '" + access_log + "' could not be opened, falling back to 'access.log'\n", STD_ERR | ERROR_FILE);

        new_server->set_access_log(ACCESS_LOG_DEFAULT);
    }

    void	configure_index(const std::vector <std::string>& index_vector, Server*& new_server)
    {
        std::string index;

        if (index.empty() == false)
        {
            index = index_vector[0];

            if (Utils::file_exists(index) == false or Utils::write_access(index) == true)
            {
                new_server->set_index_path(index);
                return ;
            }
        }
        Log::log("error: access log '" + index + "' could not be opened, falling back to 'access.log'\n", STD_ERR | ERROR_FILE);
        new_server->set_index_path("index.html");
    }

    void	configure_autoindex(const std::vector <std::string>& autoindex_vector, Server*& new_server)
    {
        if (autoindex_vector.empty() == false)
        {
            if (autoindex_vector[0] == "enabled")
            {
                new_server->set_auto_index(true);
            }
        }
    }

    void	configure_root(const std::vector <std::string>& root_vector, Server*& new_server)
    {
        if (root_vector.empty() == false)
        {
            new_server->set_root(root_vector[0]);
        }
    }

    // validates client max body size from config
    //
    // if:	the value is missing or empty
    //		->	log error & fall back to default
    //
    // else if: the value is too high
    //		->	log error & cap it to 10M
    //
    // else if: the value is invalid
    //		->	log error & fall back to default
    void	configure_client_max_body_size(const std::vector <std::string>& body_size_vector, Server*& new_server)
    {
        if (body_size_vector.empty() == true)
        {
            Log::log("no client max body size config in server '" + new_server->get_server_name()[0] + "', falling back to default (1M)\n", STD_ERR | ERROR_FILE);
            new_server->set_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT);
            return ;
        }

        std::string client_max_body_size = body_size_vector[0];
        int	size = Utils::parse_client_max_body_size(client_max_body_size);

        if (size > CLIENT_MAX_BODY_SIZE_MAX)
        {
            Log::log("error: " + client_max_body_size + ": client max body size too high, capping to 10M\n", STD_ERR | ERROR_FILE);
            size = CLIENT_MAX_BODY_SIZE_MAX;
        }
        else if (size < 0)
        {
            Log::log("error: client max body size '" + client_max_body_size + "' is not valid, falling back to default (1M)\n", STD_ERR | ERROR_FILE);
            size = CLIENT_MAX_BODY_SIZE_DEFAULT;
        }
        new_server->set_client_max_body_size(size);
    }
}