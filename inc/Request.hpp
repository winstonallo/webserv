#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>
#define READ 5
#define NOTREAD 2


class Request
{
	public:

		Request() {}
		~Request() {}

        void                                init(std::string request);
        void                                clean(void);
        
        std::map <std::string, std::string> get_headers() const { return _headers; }
        std::string                         get_method() const { return _method; }
        std::string                         get_uri() const { return _uri; }
        std::string&                        get_body() { return body; }
        std::string                         get_protocol() const { return _protocol; }
        std::string                         get_userinfo() const { return _userinfo; }
        std::string                         get_host() const { return _host; }
        std::string                         get_port() const { return _port; }
        std::string                         get_path() const { return _path; }
        std::string                         get_query() const { return _query; }
        std::string                         get_fragment() const { return _fragment; }

        int                                 get_errcode() const{ return _errcode; }
        void                                set_errcode(int code){ _errcode = code; }

        std::string                         get_header(const std::string& key) const;
        static int                          read_request(int client_fd, int size,std::string& requestmsg);
        std::string 					    body;

	private:

		std::string 					    _protocol;
        std::string 					    _method;
        std::string 					    _uri;
        std::map <std::string, std::string> _headers;
        std::string 					    _userinfo;
        std::string 					    _host;
        std::string 					    _port;
        std::string 					    _path;
        std::string 					    _query;
        std::string 					    _fragment;
        int                                 _errcode;
		void                                parse(std::string request);
        void                                validate_request(void);
        void                                validate_uri(void);
        void                                pct_decode(void);
        void                                check_length(void);
        void                                check_headers(void);

		Request(const Request&) {}
		Request&		operator=(const Request&) { return *this; }
};

int read_request(int client_fd, int size,std::string& requestmsg);

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
#define PCHAR UNRESERVED SUBDELIMS ":@%"
#define TCHAR UNRESERVED SUBDELIMS "/?#"
#define USERINFO UNRESERVED SUBDELIMS ":%"
#define FRAGMENT PCHAR "/?%"
#define QUERY PCHAR "/?%" 
#define PATH PCHAR "/%"
#define HOST UNRESERVED SUBDELIMS "%:"



#endif
