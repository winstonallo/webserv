#include <cstdlib>
#include <cstddef>
#include <string>
#include <utility>
#include <map>
#include <cstring>
#include "ConfigDispatcher.hpp"
#include "Utils.hpp"
#include "Log.hpp"
#include "Utils.hpp"

ConfigDispatcher::ConfigDispatcher(const line_count_map& raw_config) : _raw_config(raw_config)
{
	dispatch_values();
}

void	ConfigDispatcher::dispatch_values() 
{
	for (line_count_map::iterator it = _raw_config.begin(); it != _raw_config.end(); it++)
	{
		handle_server(it->first);
		handle_error_page(std::make_pair(it->first, it->second));
	}
}

void	ConfigDispatcher::handle_server(const std::string& key)
{
	size_t server_prefix_size = std::string(SERVER_PREFIX).size();

	if (key.substr(0, server_prefix_size) == SERVER_PREFIX)
	{
		int	server_id = Utils::extract_numeric_value(key.substr(server_prefix_size));

		_servers[server_id][key.substr(server_prefix_size + 3)] = _raw_config[key];
	}
}

void ConfigDispatcher::handle_error_page(const std::pair <std::string, std::pair <std::vector<std::string>, int> >& key_value)
{
    std::string file_path = key_value.first.substr(0, key_value.first.find_last_of(":"));

    if (file_path == ERROR_PAGE_PREFIX)
    {
    	std::string error_page = key_value.second.first[0];
		int status_code = std::atoi(key_value.first.substr(key_value.first.size() - 3).c_str());

		if (error_page == ".html")
		{
			Utils::config_error_on_line(key_value.second.second, file_path +  ": invalid file - hidden files not accepted, falling back to default\n");
		}
		else if (error_page.size() < 6 || error_page.substr(error_page.size() - 5) != ".html")
		{
			Utils::config_error_on_line(key_value.second.second, file_path + ": invalid file - expecting .html, falling back to default\n");
		}
		else if (Utils::file_exists(error_page))
		{
			_error_pages[status_code] = error_page;
		}
		else
		{
			Utils::config_error_on_line(key_value.second.second, file_path + ": file not found, falling back to default\n");
		}
    }
}
