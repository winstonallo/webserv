#ifndef LOCATIONINFO_HPP
#define LOCATIONINFO_HPP

#include <string>
#include <vector>

class LocationInfo 
{
	public:

		LocationInfo()
		{
			_path = "";
			_autoindex = false; 
			_root = "";
			_index = "";
			_return = "";
			_alias = "";
			_cgi_handler = "";
			_cgi_ext = std::vector<std::string>();
			_allowed_methods = std::vector<std::string>(); 
			_directory_listing_enabled = false;
			_client_max_body_size = 0;
			_cgi = false;
		}
		
		LocationInfo&				operator=(const LocationInfo& rhs)
		{
			if (this != &rhs)
			{
				_path = rhs._path;
				_autoindex = rhs._autoindex;
				_root = rhs._root;
				_index = rhs._index;
				_return = rhs._return;
				_alias = rhs._alias;
				_cgi_handler = rhs._cgi_handler;
				_cgi_ext = rhs._cgi_ext;
				_allowed_methods = rhs._allowed_methods;
				_directory_listing_enabled = rhs._directory_listing_enabled;
				_client_max_body_size = rhs._client_max_body_size;
				_cgi = rhs._cgi;
			}
			return *this;
		}

		~LocationInfo() {}

		bool						get_autoindex() const { return _autoindex; }
		void						set_autoindex(bool autoindex) { _autoindex = autoindex; }
		std::vector <std::string>	get_allowed_methods() const { return _allowed_methods; }
		void						set_allowed_methods(std::vector<std::string> allowed_methods) { _allowed_methods = allowed_methods; }
		std::string					get_root() const { return _root; }
		void						set_root(const std::string& root) { _root = root; }
		std::string					get_path() const { return _path; }
		void						set_path(const std::string& path) { _path = path; }
		std::string					get_return() const { return _return; }
		void						set_return(const std::string& ret) { _return = ret; }
		std::string					get_alias() const { return _alias; }
		void						set_alias(const std::string &alias) { _alias = alias; }
		bool						get_cgi() const { return _cgi; }
		void						set_cgi(bool cgi) { _cgi = cgi; }
		std::string					get_cgi_handler() const { return _cgi_handler; }
		void						set_cgi_handler(const std::string& cgi_handler) { _cgi_handler = cgi_handler; }
		std::vector<std::string>	get_cgi_extensions() const { return _cgi_ext; }
		void						set_cgi_extensions(std::vector<std::string> extension) { _cgi_ext = extension; }
		int							get_client_max_body_size() const { return _client_max_body_size; }
		void						set_client_max_body_size(int n) { _client_max_body_size = n; }
		std::string					get_index_path() const { return _index; }
		void						set_index_path(const std::string& loc) { _index = loc; }
		
	private:
		std::string					_path;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		std::string					_return;
		std::string					_alias;
		std::string					_cgi_handler;
		std::vector<std::string>	_cgi_ext;
		std::vector <std::string>	_allowed_methods;
		bool						_directory_listing_enabled;
		int							_client_max_body_size;
		bool						_cgi;

									LocationInfo(const LocationInfo&) {}
};

std::ostream &operator<<(std::ostream& out, const LocationInfo& rhs);

#endif
