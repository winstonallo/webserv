#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <cstddef>
#include <string>
#include <vector>
#pragma once

#include "Headers.hpp"

// typedef std::map <std::string, std::map <std::string, std::map <std::string, std::map <std::string, std::vector <std::string> > > > > config_map;
class Config 
{
	public:
	
		Config(const std::string& path="webserv.conf");
		~Config();

		void												load_config_from_file(const std::string& config_path);
		void												parse_config_from_vector(const std::vector <std::string>& config);
		void												handle_open_brace(const std::string& new_key);
		void												handle_closing_brace();
		void												store_key_value_pairs(const std::string& line);
		void												validate_nesting();
		void												validate_config_header(const std::vector <std::string>& config);

		std::map <std::string, std::vector <std::string> >	get_config() const;

	private:
	
		std::stack <std::string> 							_nesting_level;		// keeps track of nesting level
		size_t												_max_nesting_level;
		std::vector <std::string>							_keys;
		std::map <std::string, std::vector <std::string> >	_config;

		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif