#ifndef LOCATIONINFO_HPP
#define LOCATIONINFO_HPP

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
