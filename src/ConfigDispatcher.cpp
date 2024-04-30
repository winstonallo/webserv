#include <cstdlib>
#include <cstddef>
#include <string>
#include <utility>
#include <map>
#include <cstring>
#include "ConfigDispatcher.hpp"
#include "Utils.hpp"
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
	}
}

void	ConfigDispatcher::handle_server(const std::string& key)
{
	size_t server_prefix_size = std::string(SERVER_PREFIX).size();

	if (key.substr(0, server_prefix_size) == SERVER_PREFIX)
	{
		int	server_id = Utils::extract_numeric_value(key.substr(server_prefix_size));
		std::string server_scope_key = key.substr(server_prefix_size + 3);
		
		if (server_scope_key.size() > 12)
		{
			std::string subkey = server_scope_key.substr(0, 11);

			if (subkey == "error_pages")
			{
				std::vector <std::string> error_page;
				error_page.push_back(server_scope_key.substr(server_scope_key.find_first_of(":") + 1));
				error_page.push_back(_raw_config[key].first[0]);
				_servers[server_id][subkey].first = error_page;
				return ;			
			}
		}
		_servers[server_id][server_scope_key] = _raw_config[key];
	}
}
