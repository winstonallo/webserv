/* 
	config class implementation
	handles loading, parsing, and accessing config settings from .conf file
*/

#include "../inc/Config.hpp"

// default constructor: loads config from path
// 
// @param path: path to the configuration file (default: 'webserv.conf')
Config::Config(const std::string& path)
{
	if (path.substr(path.size() - 5) != ".conf") // check for valid file extension
	{
		throw std::runtime_error("'" + path + "': invalid file extension (expected: .conf)");
	}

	load_config_from_file(path);
}

std::string	Config::remove_comments(const std::string& config)
{
	std::string			cleaned_config;
	std::string			line;
	std::istringstream	input(config);

	while (std::getline(input, line))
	{
		cleaned_config += Parser::trim_comment(line, "#") + "\n";
	}
	return cleaned_config;
}

// open config file
// reads into string for easier splitting
//
// @param path: path to the configuration file (default: 'webserv.conf')
void 	Config::load_config_from_file(const std::string& path)
{
    std::ifstream 				config_file(path.c_str());
    std::stringstream 			buffer;

    buffer << config_file.rdbuf();

    config_file.close();

 	// split the config into a string vector
	// keep the delimiters for easier tracking of nesting level
	parse_config_from_vector(Parser::split_keep_delimiters(remove_comments(buffer.str()), "{};"));
}

// this assumes that the config file is correctly structured -> getting started on validation
void	Config::parse_config_from_vector(const std::vector <std::string>& config)
{
	validate_config_header(config);

	_nesting_level.push(config[0]); // initialize the nesting_level stack with the top value (webserv)

	for (size_t i = 2; i < config.size(); i++)
	{
		if (config[i] == "{") // entering new scope -> push new path to stack
		{
			handle_opening_brace(config[i - 1]);
		}
		else if (config[i] == "}") // leaving scope
		{
			handle_closing_brace(config[i - 1]);
		}
		else if (config[i] == ";") // reached 'bottom' scope -> store values
		{
			store_key_value_pairs(config[i - 1]);
		}
	}
	validate_nesting();
}

// stores the key value pairs into the correct map position
//
// @param line: bottom (value level) pair line
//
// splits the line by whitespaces, assuming the first index is the key
// stores the rest into the value vector of the map
void	Config::store_key_value_pairs(const std::string& line)
{
	std::vector <std::string> bottom_pair = Parser::split(line, " \t\n");

	_nesting_level.push(_nesting_level.top() + ":" + bottom_pair[0]);

	for (size_t j = 1; j < bottom_pair.size(); j++)
	{
		_config[_nesting_level.top()].push_back(bottom_pair[j]);
	}

	_nesting_level.pop();
}

void	Config::handle_opening_brace(const std::string& prev_line)
{
	if (prev_line.find_first_of(";{}") != std::string::npos)
	{
		throw std::runtime_error("uninitialized scope");
	}
	_nesting_level.push(_nesting_level.top() + ":" + prev_line);
}

// checks whether the stack is empty before popping
//
// if yes: throw a runtime_error
//
// else: pop top value from the stack and keep going
void	Config::handle_closing_brace(const std::string& prev_line)
{
	if (_nesting_level.empty() == true)
	{
		throw std::runtime_error("extraneous closing brace");
	}
	else if (prev_line != ";" and prev_line != "}")
	{
		throw std::runtime_error("unterminated value scope at '" + prev_line + "'");
	}
	_nesting_level.pop();
}

// validate header of the config file
//
// @param config: vector of strings with the split config
//
// checks whether the config has the right header and is opened by '{'
void	Config::validate_config_header(const std::vector <std::string>& config)
{
	if (config[0].substr(0, 7) != "webserv")
	{
        throw std::runtime_error("invalid config file header: '" + config[0] + "', please use 'webserv'");
	}

	if (config[1] != "{")
	{
		throw std::runtime_error("please use '{ }' for indentation");
	}
}

// ensures that no nesting scope is left open
void	Config::validate_nesting()
{
	if (_nesting_level.empty() == false)
	{
		throw std::runtime_error("missing closing brace");
	}
}

// @return: config map
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
		std::cout << BOLD << it->first << RESET << std::endl;

		for (std::vector <std::string>::iterator its = it->second.begin(); its != it->second.end(); its++)
		{
			std::cout << "\t" << (*its) << std::endl; 
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