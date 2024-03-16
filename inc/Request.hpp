#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include "Utils.hpp"


class Request
{
	public:
        // constructors and destructors
									Request(std::string request);
									~Request();
									Request(const Request& rhs);
		Request&					operator=(const Request& rhs);

        // init and parse request
        void                        parse(std::string request);
        void                        validate(void);
        std::string                 validate_uri(void);

        // setters and getters
        std::string 				        			get_protocol() const;
        std::string 				        			get_method() const;
        std::string 				        			get_uri() const;
        std::map <std::string, std::string>             get_headers() const;
        std::string 									get_body() const;
        std::string 									get_header(const std::string& key) const;
        std::string 									get_userinfo() const;
		std::string 									get_host() const;
		std::string 									get_port() const;
		std::string 									get_path() const;
		std::string 									get_query() const;
		std::string 									get_fragment() const;

	protected:
		std::string 					protocol;
        std::string 					method;
        std::string 					uri;
        std::map <std::string, std::string>             headers;
        std::string 					body;
        std::string 					userinfo;
        std::string 					host;
        std::string 					port;
        std::string 					path;
        std::string 					query;
        std::string 					fragment;

};
std::ostream& operator<<(std::ostream& os, const Request& req);

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define RED "\033[31m"
#define GREEN "\033[32m"

#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT "0123456789"
#define HEXDIG "0123456789ABCDEFabcdef"
#define UNRESERVED ALPHA DIGIT "-._~"
#define SUBDELIMS "!$&'()*+,;="
#define PCHAR UNRESERVED SUBDELIMS ":@"
#define TCHAR UNRESERVED SUBDELIMS "/?#"
#define USERINFO UNRESERVED SUBDELIMS ":"
#define FRAGMENT PCHAR "/?"
#define QUERY PCHAR "/?" 
#define PATH PCHAR "/"



#endif
