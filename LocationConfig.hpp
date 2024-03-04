/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 18:14:48 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:16:15 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <iostream>

class LocationConfig 
{
	public:
							LocationConfig();
							~LocationConfig();
							LocationConfig(const LocationConfig& rhs);
		LocationConfig&		operator=(const LocationConfig& rhs);
		std::string			getRoot() const;
		void				setRoot(std::string rt);
		bool				getExpires() const;
		void				setExpires(const bool& texpires);
		

	private:
		std::string			root;
		bool				expires;		
};

#endif
