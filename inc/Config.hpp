/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abied-ch <abied-ch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 22:12:53 by abied-ch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Headers.hpp"

class Config 
{
	public:
	
		Config(const std::string& path = "default_config.conf");
		~Config();

		void									load_config(const std::string& config_path);
		void									map_line(const std::string& line);
		void									set_server_names();
		void									set_allowed_methods();

		std::string								get_value(const std::string& key);
		std::map <std::string, std::string> 	get_config() const;
		std::vector <std::string>				get_server_names() const;
		std::vector <std::string>				get_allowed_methods() const;
		void									store_values();

	private:
	
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
		
		std::map <std::string, std::string> config;
		std::vector <std::string>			server_names;
		std::vector <std::string>			allowed_methods;
};

#endif