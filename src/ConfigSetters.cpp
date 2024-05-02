#include <cstdlib>
#include <stdexcept>
#include "ConfigSetters.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Log.hpp"
#include "Utils.hpp"

namespace Setters
{
    template <typename T>
    void	set_string_value(const std::vector <std::string>& value, T* target, void (T::*setter)(const std::string&))
    {
        if (value.empty() == false)
        {
            (target->*setter)(value[0]);
        }
    }

    template <typename T>
    void    set_bool_value(const std::vector <std::string>& value, T* target, void (T::*setter)(bool))
    {
        if (value.empty() == false)
        {
            if (value[0] == "enabled")
            {
                (target->*setter)(true);
            }
            else if (value[0] == "disabled")
            {
                (target->*setter)(false);
            }
            else 
            {
                Utils::config_error_on_line(-1, "Invalid value for boolean config, use enabled/disabled.");
            }
        }
    }

    template <typename T>
    void    set_vector_value(const std::vector <std::string>& value, T* target, void (T::*setter)(const std::vector<std::string>))
    {
        if (value.empty() == false)
        {
            (target->*setter)(value);
        }
    }

    template <typename T>
    void    set_string_check_path(const std::vector <std::string>& value, T* target, void (T::*setter)(const std::string&), const std::string& dflt)
    {
        if (value.empty() == false)
        {
            if (Utils::file_exists(value[0]) == true)
            {
                (target->*setter)(value[0]);
            }
            else
            {
                (target->*setter)(dflt);
            }
        }
        else
        {
            (target->*setter)(dflt);
        }
    }

    void	set_root(const std::vector <std::string>& root, LocationInfo*& new_location)
    {
        set_string_value(root, new_location, &LocationInfo::set_root);
    }

    void	set_allowed_methods(const std::vector <std::string>& allowed_methods, LocationInfo*& new_location)
    {
        set_vector_value(allowed_methods, new_location, &LocationInfo::set_allowed_methods);
    }

    void	set_return(const std::vector <std::string>& rtrn, LocationInfo*& new_location)
    {
        set_string_value(rtrn, new_location, &LocationInfo::set_return);
    }

    void	set_alias(const std::vector <std::string>& alias, LocationInfo*& new_location)
    {
        set_string_value(alias, new_location, &LocationInfo::set_alias);
    }

    void	set_cgi_handler(const std::vector <std::string>& cgi_path, LocationInfo*& new_location)
    {
        if (access(cgi_path[0].c_str(), X_OK) == -1)
        {
            throw std::runtime_error("CGI handler '" + cgi_path[0] + "' is not executable.");
        }
        set_string_value(cgi_path, new_location, &LocationInfo::set_cgi_handler);
    }

    void	set_cgi_extension(const std::vector <std::string>& extension, LocationInfo*& new_location)
    {
        set_vector_value(extension, new_location, &LocationInfo::set_cgi_extensions);
    }

    void	set_autoindex(const std::vector <std::string>& autoindex, LocationInfo*& new_location)
    {
        set_bool_value(autoindex, new_location, &LocationInfo::set_autoindex);

        if (new_location->get_cgi() == true and new_location->get_autoindex() == true)
        {
            Utils::config_error_on_line(-1, "'autoindex' is not a valid setting for cgi-bin.");
        }
    }

    void	set_index(const std::vector <std::string>& index, LocationInfo*& new_location)
    {
        set_string_value(index, new_location, &LocationInfo::set_index_path);

        if (new_location->get_cgi() == true)
        {
            Utils::config_error_on_line(-1, "'index' is not a valid setting for cgi-bin.");
        }
    }

    void	configure_access_log(const std::vector <std::string>& access_log_vector, Server*& new_server)
    {
        set_string_check_path(access_log_vector, new_server, &Server::set_access_log, "access.log");
    }

    void	configure_index(const std::vector <std::string>& index_vector, Server*& new_server)
    {
        set_string_value(index_vector, new_server, &Server::set_index_path);
    }

    void	configure_autoindex(const std::vector <std::string>& autoindex_vector, Server*& new_server)
    {
        set_bool_value(autoindex_vector, new_server, &Server::set_auto_index);
    }

    void	configure_root(const std::vector <std::string>& root_vector, Server*& new_server)
    {
        set_string_value(root_vector, new_server, &Server::set_root);
    }

    void    add_error_page(const std::vector <std::string>& error_page, Server*& new_server)
    {
        try
        {
            Utils::safe_ifstream(error_page[1]);
            new_server->add_error_page(std::atoi(error_page[0].c_str()), error_page[1]);
        }
        catch (const std::exception& e)
        {
            Log::log(e.what(), STD_ERR | ERROR_FILE);
        }
    }

    void	configure_client_max_body_size(const std::vector <std::string>& body_size_vector, Server*& new_server)
    {
        if (body_size_vector.empty() == true)
        {
            Log::log("No client max body size configured in server '" + new_server->get_server_name()[0] + "', default is 1M.");
            new_server->set_client_max_body_size(CLIENT_MAX_BODY_SIZE_DEFAULT);
            return ;
        }

        std::string client_max_body_size = body_size_vector[0];
        int	size = Utils::parse_client_max_body_size(client_max_body_size);

        if (size > CLIENT_MAX_BODY_SIZE_MAX)
        {
            Utils::config_error_on_line(-1, "Client max body size too high, capping to 16M.");
            size = CLIENT_MAX_BODY_SIZE_MAX;
        }
        else if (size < 0)
        {
            Utils::config_error_on_line(-1, "Client max body size '" + client_max_body_size + "' is not valid, falling back to default (1M).");
            size = CLIENT_MAX_BODY_SIZE_DEFAULT;
        }
        new_server->set_client_max_body_size(size);
    }
}