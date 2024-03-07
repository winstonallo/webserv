/* 
	config class implementation
	handles loading, parsing, and accessing config settings from a file
*/

#include "../inc/Config.hpp"

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

	_nesting_level.push(""); // init stack

    for (size_t i = 2; i < config.size(); ++i) 
	{
		process_config_elements(config, i);

		validate_nesting_depth_limit();
    }
	if (_nesting_level.size() != 0)
	{
		throw std::runtime_error("missing closing brace in config");
	}
	_max_nesting_level = 0;

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

void	Config::process_config_elements(const std::vector <std::string>& config, const size_t i)
{
	if (config[i] == "{")
	{
		handle_open_brace(config[i - 1]);
	} 
	else if (config[i] == "}")
	{
		handle_closing_brace();
	} 
	else if (config[i] != ";") // reached value level -> store in map
	{
		store_key_value_pairs(config[i], _keys);
	}
}

// handle opening brackets
//
// @param new_key: key to update the key map with
//
// pushes to the stack, signifying additional level of nesting
void	Config::handle_open_brace(const std::string& new_key)
{
	_nesting_level.push("");

	_keys[_nesting_level.size() - 1] = new_key;
}

// handle closing brackets
//
// if stack is not empty, we pop it and update the key map
// else if it is empty -> one too many closing brackets -> throw error
void	Config::handle_closing_brace()
{
	if (_nesting_level.empty() == false) 
	{
		_keys[_nesting_level.size() - 1] = "";

		_nesting_level.pop();
	}
	else
	{
		throw std::runtime_error("extraneous closing brace in config file");
	}
}

void	Config::store_key_value_pairs(const std::string& line, std::vector <std::string>& keys)
{
	std::vector<std::string> temp = Parser::split(line, " \t\n");

	if (temp.empty() == false) 
	{
		for (size_t k = _nesting_level.size(); k < keys.size() - 1 ; ++k)
			keys[k] = "";
		for (size_t j = 1; j < temp.size(); ++j)
		{
			set_default_keys(keys);
			_config[keys[0]][keys[1]][keys[2]][temp[0]].push_back(temp[j]);
		}
	}
}

void	Config::set_default_keys(std::vector <std::string>& keys)
{
	if (keys[0].empty())
		keys[0] = "__top_level";

	if (keys[1].empty())
		keys[1] = "__sec_level";
	
	if (keys[2].empty())
		keys[2] = "__server_specs";
}

void	Config::validate_nesting_depth_limit()
{
	_max_nesting_level = std::max(_nesting_level.size(), _max_nesting_level);
	
	if (_max_nesting_level > 5)
	{
		throw std::runtime_error("no need to nest your config this deep man");
	}
}

config_map	Config::get_config() const
{
	return _config;
}

// output operator overload for debugging
std::ostream& operator<<(std::ostream& os, const Config& config) 
{
	config_map map = config.get_config();
	for (config_map::iterator it = map.begin(); it != map.end(); ++it)
	{
		std::cout << "" << BOLD << UNDERLINE << it->first << RESET << "\n" << std::endl;
		for (std::map <std::string, std::map <std::string, std::map <std::string, std::vector <std::string> > > >::iterator yo = it->second.begin(); yo != it->second.end(); yo++)
		{
			std::cout << "\t" << BOLD << UNDERLINE << yo->first << RESET << "\n" << std::endl;
			for (std::map <std::string, std::map <std::string, std::vector <std::string> > >::iterator i = yo->second.begin(); i != yo->second.end(); i++)
			{
				std::cout << "\t\t" << BOLD << UNDERLINE << i->first << RESET << "\n" << std::endl;
				for (std::map<std::string, std::vector<std::string> >::iterator it2 = i->second.begin(); it2 != i->second.end(); ++it2)
				{
					std::cout << "\t\t\t" << BOLD << it2->first << RESET << std::endl;
					for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
					{
						std::cout << "\t\t\t\t" << *it3 << std::endl;
					}
				}
			}
		}
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