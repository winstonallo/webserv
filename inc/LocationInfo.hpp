/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationInfo.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abied-ch <abied-ch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:58:09 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/12 20:51:22 by abied-ch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOCATIONINFO_HPP
#define LOCATIONINFO_HPP
#pragma once

#include "Headers.hpp"
class LocationInfo 
{
	public:
							LocationInfo();
							~LocationInfo();
							LocationInfo(const LocationInfo& rhs);
		LocationInfo&		operator=(const LocationInfo& rhs);
		std::string			getPath() const;
		void				setPath(std::string p);
		bool				getAutoindex() const;
		void				setAutoindex(bool b); 
		
	private:
		std::string			path;
		bool				autoindex;
};

#endif
