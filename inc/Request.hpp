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

        // setters and getters
        std::string 				        			get_protocol() const;
        void 											set_protocol(const std::string& val);
        std::string 				        			get_method() const;
        void 											set_method(const std::string& val);
        std::string 				        			get_uri() const;
        void 											set_uri(const std::string& val);
        std::map <std::string, std::string>             get_headers() const;
        void 											set_headers(const std::map <std::string, std::string>& val);
        std::string 									get_body() const;
        void 											set_body(const std::string& val);
        std::string 									get_header(const std::string& key) const;

	protected:
		std::string 					protocol;
        std::string 					method;
        std::string 					uri;
        std::map <std::string, std::string> headers;
        std::string 					body;

};
std::ostream& operator<<(std::ostream& os, const Request& req);

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define RED "\033[31m"

#endif
