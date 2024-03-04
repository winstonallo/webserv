/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:13:48 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"


Config::Config()
{

}

Config::~Config()
{

}

Config::Config(const Config& rhs)
{
	*this = rhs;
}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
	{
		servconf = rhs.servconf;
	}
	return (*this);
}
