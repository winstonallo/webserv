/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationInfo.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:58:09 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/08 12:03:24 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef LOCATIONINFO_HPP
#define LOCATIONINFO_HPP

#include <iostream>
#include <string>


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
