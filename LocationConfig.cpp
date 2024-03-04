/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 18:14:48 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:17:09 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"


LocationConfig::LocationConfig()
{

}

LocationConfig::~LocationConfig()
{

}

LocationConfig::LocationConfig(const LocationConfig& rhs)
{
	*this = rhs;
}

LocationConfig&	LocationConfig::operator=(const LocationConfig& rhs)
{
	if (this != &rhs)
	{
		root = rhs.root;
		expires = rhs.expires;
	}
	return (*this);
}

std::string	LocationConfig::getRoot() const
{
	return (root);
}

bool	LocationConfig::getExpires() const
{
	return (expires);
}

void	LocationConfig::setExpires(const bool& texpires)
{
	expires = texpires;
}

void	LocationConfig::setRoot(std::string rt)
{
	root = rt; 
}

