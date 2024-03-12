/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationInfo.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abied-ch <abied-ch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:58:09 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/12 20:48:47 by abied-ch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../inc/Headers.hpp"

LocationInfo::LocationInfo()
{

}

LocationInfo::~LocationInfo()
{

}

LocationInfo::LocationInfo(const LocationInfo& rhs)
{
	*this = rhs;
}

LocationInfo&	LocationInfo::operator=(const LocationInfo& rhs)
{
	if (this != &rhs)
	{
		path = rhs.path;
		autoindex = rhs.autoindex;
	}
	return (*this);
}

std::string	LocationInfo::getPath() const
{
	return (path);
}

void	LocationInfo::setPath(std::string p)
{
	path = p;
}

bool	LocationInfo::getAutoindex() const
{
	return (autoindex);
}

void	LocationInfo::setAutoindex(bool val)
{
	autoindex = val;
}



