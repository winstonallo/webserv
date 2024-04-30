
#include "ConfigParser.hpp"
#include "Utils.hpp"
#include <sstream>
#include <fstream>

ConfigParser::ConfigParser(const std::string& path) : _config_file_path(path), _server_count(0)
{
	if (_config_file_path.size() < 5 or _config_file_path.substr(_config_file_path.size() - 5) != EXPECTED_EXT)
	{
		Utils::config_error_on_line(0, INVALID_EXT, THROW);
	}

	load_config_from_file(_config_file_path);
	dispatch_servers();
}

std::string	ConfigParser::remove_comments(const std::string& config)
{
	std::string			cleaned_config;
	std::string			line;
	std::istringstream	input(config);

	while (std::getline(input, line))
	{
		cleaned_config += Utils::trim_comment(line, "#") + "\n";
	}
	return cleaned_config;
}

void 	ConfigParser::load_config_from_file(const std::string& path)
{
    std::ifstream 				config_file(path.c_str());
    std::stringstream 			buffer;

	if (config_file.is_open() == false)
	{
		Utils::config_error_on_line(0, error(NOT_FOUND), THROW);
	}

    buffer << config_file.rdbuf();

	if (buffer.str().empty() == true)
	{
		Utils::config_error_on_line(0, EMPTY, THROW);
	}

    config_file.close();

	parse_config_from_vector(Utils::split_keep_delimiters(remove_comments(buffer.str()), "{};"));
}

void	ConfigParser::parse_config_from_vector(const std::vector <std::pair <std::string, int> >& config)
{
	validate_config_header(config);

	_nesting_level.push(config[0].first);

	for (size_t i = 2; i < config.size(); i++)
	{
		if (config[i].first == "{")
		{
			handle_opening_brace(config[i - 1]);
		}
		else if (config[i].first == "}")
		{
			handle_closing_brace(config[i - 1]);
		}
		else if (config[i].first == ";")
		{
			store_key_value_pairs(config[i - 1]);
		}
	}
	validate_nesting(config[config.size() - 1].second + 1);
}

void	ConfigParser::store_key_value_pairs(const std::pair <std::string, int> line)
{
	if (line.first.find("\n") != std::string::npos)
	{
		Utils::config_error_on_line(line.second + 1, UNEXPECTED_NL, THROW);
	}

	std::vector <std::string> bottom_pair = Utils::split_keep_quoted_words(line.first, " \t");

	_nesting_level.push(_nesting_level.top() + ":" + bottom_pair[0]);

	if (_nesting_level.top().find("server") == std::string::npos)
	{
		Utils::config_error_on_line(line.second, INVALID_SCOPE);
		_nesting_level.pop();
		return ;
	}

	std::vector <std::string> value = bottom_pair;
	value.erase(value.begin());
	_config[_nesting_level.top()] = std::make_pair(value, line.second);

	_nesting_level.pop();
}

void	ConfigParser::handle_opening_brace(const std::pair <std::string, int>& prev_line)
{
	std::string name = prev_line.first;

	if (name == "server")
	{
		name += "_" + Utils::itoa(_server_count);
		_server_count++;
	}
	if (prev_line.first.find_first_of(";{}") != std::string::npos)
	{
		Utils::config_error_on_line(prev_line.second + 1, UNINITIALIZED_SCOPE, THROW);
	}
	_nesting_level.push(_nesting_level.top() + ":" + name);
}

void	ConfigParser::handle_closing_brace(const std::pair <std::string, int>& prev_line)
{
	if (_nesting_level.empty() == true)
	{
		Utils::config_error_on_line(prev_line.second + 1, EXTRA_CLOSING_BRACE, THROW);
	}
	else if (prev_line.first.find_first_of("{};") == std::string::npos)
	{
		Utils::config_error_on_line(prev_line.second + 1, UNTERM_VALUE_SCOPE, THROW);
	}
	_nesting_level.pop();
}

void	ConfigParser::validate_config_header(const std::vector <std::pair <std::string, int> >& config)
{
	if (config[0].first == UNCLOSED_QUOTE)
	{
		Utils::config_error_on_line(config[0].second, UNCLOSED_QUOTE, THROW);
	}

	if (Utils::trim(config[0].first.substr(0, 7), " \t\n") != "webserv")
	{
		Utils::config_error_on_line(config[0].second, INV_HEADER, THROW);
	}

	if (config[1].first != "{")
	{
		Utils::config_error_on_line(config[1].second, MISSING_OPENING_BRACE, THROW);
	}
}

void	ConfigParser::validate_nesting(int line_count)
{
	if (_nesting_level.empty() == false)
	{
		Utils::config_error_on_line(line_count, MISSING_CLOSING_BRACE, THROW);
	}
}

std::string	ConfigParser::error(const std::string& message)
{
	std::string error = strerror(errno);

	if (error == "Success")
	{
		error = "";
	}

	return _config_file_path + ": " + message + ": " + error;
}

void	ConfigParser::dispatch_servers() 
{
	for (line_count_map::iterator it = _config.begin(); it != _config.end(); it++)
	{
		store_server_values_in_map(it->first);
	}
}

void	ConfigParser::store_server_values_in_map(const std::string& key)
{
	size_t server_prefix_size = std::string(SERVER_PREFIX).size();

	if (key.substr(0, server_prefix_size) == SERVER_PREFIX)
	{
		int	server_id = Utils::extract_numeric_value(key.substr(server_prefix_size));
		std::string server_scope_key = key.substr(server_prefix_size + 3);
		
		if (server_scope_key.find("error_pages:") != std::string::npos)
		{
			std::string subkey = server_scope_key.substr(0, 11);

			if (subkey == "error_pages")
			{
				std::vector <std::string> error_page;

				error_page.push_back(server_scope_key.substr(server_scope_key.find_first_of(":") + 1));
				error_page.push_back(_config[key].first[0]);
				_servers[server_id][subkey].first = error_page;
				return ;
			}
		}
		_servers[server_id][server_scope_key] = _config[key];
	}
}
