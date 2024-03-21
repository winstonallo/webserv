#ifndef LOCATIONINFO_HPP
#define LOCATIONINFO_HPP

#include <string>
#include <vector>

class LocationInfo 
{
	public:
									LocationInfo();
									~LocationInfo();
									LocationInfo(const LocationInfo& rhs);
		LocationInfo&				operator=(const LocationInfo& rhs);
		std::string					getPath() const;
		void						setPath(std::string p);
		bool						getAutoindex() const;
		void						setAutoindex(bool b);
		std::vector <std::string>	get_allowed_methods() const;
		void						set_allowed_methods(std::vector <std::string>& allowed_methods);
		bool						directory_listing_enabled() const;
		void						set_directory_listing(bool directory_listing_enabled);
		std::string					get_name() const;
		void						set_name(const std::string& name);
		
		
	private:
		std::string					path;
		bool						autoindex;
		std::string					_name;
		bool						_directory_listing_enabled;
		std::vector <std::string>	_allowed_methods;
};

#endif
