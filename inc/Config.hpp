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
		void									line_to_map(const std::string& line);

		std::string								get_value(const std::string& key);
		std::map <std::string, std::string> 	get_config() const;

	private:
	
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
		
		std::map <std::string, std::string> config;
};

#endif