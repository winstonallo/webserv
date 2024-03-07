/* 
	config class implementation
	handles loading, parsing, and accessing config settings from a file
*/

#include "../inc/Config.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

// default constructor: loads config from path - "default_config.conf" if no param
Config::Config(const std::string& path) : _max_nesting_level(0), _keys(std::vector <std::string>(5))
{
	if (path.substr(path.size() - 5) != ".conf") // check file extension
		throw std::runtime_error("accepted config file format: .conf");

	load_config_from_file(path);
}

// open config file & read into string for easier splitting
void Config::load_config_from_file(const std::string& path)
{
    std::ifstream 				config_file(path.c_str());
    std::stringstream 			buffer;

    buffer << config_file.rdbuf();

    config_file.close();

	parse_config_from_vector(Parser::split_keep_delimiters(buffer.str(), "{};"));
}

void	Config::parse_config_from_vector(const std::vector <std::string>& config)
{
	validate_config_header(config);

	_nesting_level.push(config[0]);

	for (size_t i = 2; i < config.size(); i++)
	{
		if (config[i] == "{")
		{
			_nesting_level.push(_nesting_level.top() + ":" + config[i - 1]);
		}
		else if (config[i] == "}")
		{
			if (_nesting_level.empty() == true)
				throw std::runtime_error("extraneous closing brace");
			_nesting_level.pop();
		}
		else if (config[i] == ";")
		{
			std::vector <std::string> bottom_pair = Parser::split(config[i - 1], " \t\n");

			_nesting_level.push(_nesting_level.top() + ":" + bottom_pair[0]);

			for (size_t j = 1; j < bottom_pair.size(); j++)
				_config[_nesting_level.top()].push_back(bottom_pair[j]);

			_nesting_level.pop();
		}
	}
	std::cout << *this;
}

// validate header of the config file
//
// @param config: vector of strings with the split config
//
// checks whether the config has the right header and is opened with '{'
void	Config::validate_config_header(const std::vector <std::string>& config)
{
	if (config[0].substr(0, 7) != "webserv")
        throw std::runtime_error("please use the correct config header (webserv)");

	if (config[1] != "{")
		throw std::runtime_error("please use '{ }' for indentation");
}

std::map <std::string, std::vector <std::string> >	Config::get_config() const
{
	return _config;
}

// output operator overload for debugging
std::ostream& operator<<(std::ostream& os, const Config& config) 
{
	std::map <std::string, std::vector <std::string> > map = config.get_config();

	for (std::map <std::string, std::vector <std::string> >::iterator it = map.begin(); it != map.end(); it++)
	{
		std::cout << "key: " << it->first << std::endl;
		std::cout << "\tvalue(s): " << std::endl;
		for (std::vector <std::string>::iterator its = it->second.begin(); its != it->second.end(); its++)
			std::cout << "\t\t" << (*its) << std::endl; 
	}

	return os;
}

Config::~Config() {}

Config::Config(const Config& rhs) {*this = rhs;}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
		_config = rhs._config;
	return (*this);
}