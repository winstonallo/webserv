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
		std::string					getPath() const;
		void						set_root(const std::vector <std::string>& root);
		std::vector <std::string>	get_allowed_methods() const;
		void						set_allowed_methods(const std::vector <std::string>& allowed_methods);
		bool						directory_listing_enabled() const;
		void						set_directory_listing(bool directory_listing_enabled);

		void						set_directory_listing(const std::vector <std::string>& directory_listing_enabled);
		std::string					get_name() const;
		void						set_return(const std::string& r);
		std::string					get_return() const;
		void						set_alias(const std::string& a);
		std::string					get_alias() const;
		void						set_cgi_path(std::vector<std::string> p);	
		std::vector<std::string>	get_cgi_path() const;
		void						set_cgi_extension(std::vector<std::string> e);
		std::vector<std::string>	get_cgi_extension() const;
		void						set_name(const std::string& name);
		int							get_client_max_body_size() const;
		void						set_client_max_body_size(int n);
		
		
	private:
		std::string					_path;
		std::string					_root;
		bool						_autoindex;
		std::string					_return;
		std::string					_alias;
		std::string					_name;
		std::vector<std::string>	_cgi_path;
		std::vector<std::string>	_cgi_ext;
		std::vector <std::string>	_allowed_methods;
		bool						_directory_listing_enabled;
		int							_client_max_body_size;
};

std::ostream &operator<<(std::ostream& out, const LocationInfo& rhs);

#endif
