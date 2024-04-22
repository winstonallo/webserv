
#include "ConfigParser.hpp"
#include "Utils.hpp"
#include <sstream>
#include <fstream>

// checks the file extension against expected value and passes the file path to the parser
ConfigParser::ConfigParser(const std::string& path) : _config_file_path(path), _server_count(0)
{
	if (_config_file_path == "")
	{
		_config_file_path = "config/webserv.conf";
	}
	if (_config_file_path.substr(_config_file_path.size() - 5) != EXPECTED_EXT)
	{
		Utils::config_error_on_line(0, INVALID_EXT, THROW);
	}

	load_config_from_file(_config_file_path);
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

	std::vector <std::string> value = bottom_pair;
	value.erase(value.begin());
	_config[_nesting_level.top()] = std::make_pair(value, line.second);

	if (_nesting_level.top().substr(0, 20) == "webserv:error_pages:")
	{
		int status_code = std::atoi(_nesting_level.top().substr(_nesting_level.top().size() - 3).c_str());
		_error_pages[status_code] = _config[_nesting_level.top()].first[0];
	}

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
