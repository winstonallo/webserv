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
		bool						getAutoindex() const;
		void						setAutoindex(bool b);
		std::string					get_root() const;
		void						set_root(const std::vector <std::string>& root);
		std::vector <std::string>	get_allowed_methods() const;
		void						set_allowed_methods(const std::vector <std::string>& allowed_methods);
		bool						directory_listing_enabled() const;
		void						set_directory_listing(const std::vector <std::string>& directory_listing_enabled);
		std::string					get_name() const;
		void						set_name(const std::string& name);
		
		
	private:

		bool						autoindex;
		std::string					_root;
		std::string					_name;
		bool						_directory_listing_enabled;
		std::vector <std::string>	_allowed_methods;
};

#endif
