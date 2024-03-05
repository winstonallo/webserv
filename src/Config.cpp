/* 
	config class implementation
	handles loading, parsing, and accessing config settings from a file
 */

#include "../inc/Config.hpp"

// default constructor: loads config from path - "default_config.conf" if no param
Config::Config(const std::string& path)
{
	load_config(path);
}

// reads & stores config from path
//
// @param 	config_path: path to config file
//
// opens config_file and passes it to load_file_to_map for parsing
void	Config::load_config(const std::string &config_path) 
{    
    std::ifstream 	config_file(config_path.c_str());
    
	if (config_file.is_open() == false)
		throw std::runtime_error("could not open config file");

	load_file_to_map(config_file);

	std::cout << *this;
    config_file.close();
}

// loads config file into structured map
//
// @param 	config_file: input file stream opened with the config file
//
// parses each line of the file, extracts & organizes them into
// hierarchical map structure for easy access
void	Config::load_file_to_map(std::ifstream& config_file)
{
	std::string line;
	std::string prev_line;
	std::string key;
	
	while (std::getline(config_file, line))
	{
        line = Parser::trim_comment(Parser::trim(line, " \t\n"), "#;");
		
        if (line.empty())
            continue ;
		// get the map key we want to assign the line to
		key = get_primary_key(line, prev_line, key);
		// store the line in the map
		load_line_to_map(line, key);
		
		prev_line = line;
    }
}

// determines the appropriate primary key for storing configuration settings 
// based on the current and previous lines
std::string	Config::get_primary_key(const std::string& line, const std::string& prev_line, std::string& key)
{
	if (line[line.size() - 1] == '{') 
	{
		if (line.size() == 1) // if '{' is standalone, get key from previous line & special-case 'server'
		{
			return prev_line;
		}
		else // if '{' follows text, trim & use previous line as the key
		{
			return Parser::trim(line, "{ \t\n");
		}
	}
	else if (line[line.size() - 1] == '}')
		return TOP_LEVEL;
	return key; // no change in status -> return current key
}

// parses config line and stores it in map under appropriate keys
//
// takes a single line of the config & splits it into components
// to store it hierarchically
// expects the line to contain a key-value pair, where the key is the first
// element, and subsequent ones are the values
//
// @param 	line: config line to be parsed
// @param 	primary_key: map key where the result will be stored 
// 
// @operations:
// 		1. 	splits trimmed line into individual components based on config delimiters
// 		2. 	uses the first element of the split as the secondary key within the 
// 			current primary scope
void		Config::load_line_to_map(const std::string& line, std::string& primary_key)
{
	std::vector <std::string>	key_values = Parser::split(line, " \t\n;{}");
	
	for (size_t i = 1; i < key_values.size(); i++) // use index 0 as the secondary key & the rest as the value
	{
		config[primary_key][key_values[0]].push_back(key_values[i]);
	}
}

// returns value from the config map
//
// @param 	primary_key: primary key for required value (TOP_LEVEL for top level values)
// @param 	secondary_key: secondary key for required value (eg: 'listen'for the port)
// @return	value: requested value
std::vector <std::string>	Config::get_value(const std::string& primary_key, const std::string& secondary_key)
{
	return config[primary_key][secondary_key];
}

// returns the config map
config_map	Config::get_config() const
{
	return config;
}

// output operator overload for debugging
std::ostream& operator<<(std::ostream& os, const Config& config) 
{
	config_map map = config.get_config();
	for (config_map::iterator it = map.begin(); it != map.end(); ++it)
	{
		std::cout << BOLD << UNDERLINE << it->first << RESET << "\n" << std::endl;
		for (std::map<std::string, std::vector<std::string> >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			std::cout << "\t" << BOLD << it2->first << RESET << std::endl;
			for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
			{
				std::cout << "\t\t" << *it3 << std::endl;
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
		config = rhs.config;
	return (*this);
}