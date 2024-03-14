#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Headers.hpp"
#include "Message.hpp"

class Request
{
	public:
									Request(std::string request);
									~Request();
									Request(const Request& rhs);
		Request&					operator=(const Request& rhs);

        std::string 				get_protocol() const;
        void 						set_protocol(const std::string& val);
        std::string 				get_method() const;
        void 						set_method(const std::string& val);
        std::string 				get_uri() const;
        void 						set_uri(const std::string& val);
        std::map <std::string, std::string> get_headers() const;
        void 						set_headers(const std::map <std::string, std::string>& val);
        std::string 				get_body() const;
        void 						set_body(const std::string& val);
        std::string 				get_header(const std::string& key) const;

	protected:
		std::string 					protocol;
        std::string 					method;
        std::string 					uri;
        std::map <std::string, std::string> headers;
        std::string 					body;

};


#endif
