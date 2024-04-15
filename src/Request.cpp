#include <iostream>
#include "Utils.hpp"
#include "Config.hpp"
#include "Request.hpp"

Request::~Request(){}

std::ostream& operator<<(std::ostream& os, const Request& req)
{
    // print request with name in red
    os << RED << "~FULL REQUEST~ " << std::endl;
    os << RED << "Host: " << RESET << req.get_host() << std::endl;
    os << RED << "Port: " << RESET << req.get_port() << std::endl;
    os << RED <<"Method: " << RESET << req.get_method() << std::endl;
    os << RED << "URI: " << RESET << req.get_uri() << std::endl;
    os << RED << "Protocol: " << RESET  << req.get_protocol() << std::endl;
    os << RED << "Headers: " << RESET << std::endl;
    std::map <std::string, std::string> headers = req.get_headers();
    for (std::map <std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        os << "\t" << it->first << ": " << it->second << std::endl;
    }
    os << RED  << "Body:\n" << RESET;
    std::stringstream ss(req.get_body());
    std::string line;
    while (std::getline(ss, line)) {
        os << "\t" << line << std::endl;
    }
    os << GREEN << "BODY SIZE: "<< RESET << req.get_body().size() << std::endl;
    return os;
}
std::string Request::get_header(const std::string& key) const
{
    if (this->headers.find(key) != this->headers.end())
    {
    	return this->headers.at(key);
    }
    return "default";
}

// init and parse request


// parse request
//
// @param request: request string
void Request::parse(std::string request){
    std::string line;
    std::istringstream iss(request);
    std::getline(iss, line);
    //check that there are only 2 spaces in the line
    if (std::count(line.begin(), line.end(), ' ') != 2)
    {
        this->errcode = 400;
        throw std::runtime_error("Invalid request: line does not contain 2 spaces between method, uri and protocol");
    }
    //check that there are now white spaces in the line except for the 2 spaces
    for (size_t i = 0; i < line.size() - 2; i++)
    {
        if (std::isspace(line[i]) != 0 && line[i] != ' ')
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid request: line contains white spaces except for the 2 spaces between method, uri and protocol");
        }
    }

    //check that line ends with \r\n
    if (line[line.size() - 1] != '\r')
    {
        this->errcode = 400;
        throw std::runtime_error("Invalid request: line does not end with \\r");
    }
    std::istringstream iss_line(line);
    std::string method;
    std::string uri;
    std::string protocol;
    // get method, uri, protocol and check that they are not empty
    iss_line >> method;
    iss_line >> uri;
    iss_line >> protocol;
    if (method.size() == 0 || uri.size() == 0 || protocol.size() == 0)
    {
        this->errcode = 400;
        throw std::runtime_error("Invalid request: method, uri or protocol is empty");
    }
    this->method = method;
    this->uri = uri;
    this->protocol = protocol;

    // get headers
    while (std::getline(iss, line))
    {
        if (line == "\r")
        {
            break;
        }
        std::istringstream iss_line(line);
        std::string key;
        std::string value;
        std::getline(iss_line, key, ':');
        std::getline(iss_line, value);
        if (value.end()[-1] != '\r')
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid request: header does not end with \\r");
        }
        //key to upper
        for (size_t i = 0; i < key.size(); i++)
        {
            key[i] = std::toupper(key[i]);
        }
        if (this->headers.find(key) != this->headers.end())
        {
            value = this->headers[key] + "," + Utils::trim(value, " \t\n\r");
            this->headers[key] = value;
        }
        else{
            this->headers[key] = Utils::trim(value, " \t\n\r");}
        
    }
    // get body
    while (std::getline(iss, line))
    {
        this->body += line + "\n";
    }

}

// goes through the string and checks if all characters are in valid_chars
//
// @param valid_chars: string of valid characters
// @param str: string to validate
// @return: true if valid, false otherwise
static bool valid_token(std::string str, std::string valid_chars)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        int c = str[i];
        if (valid_chars.find(c) == std::string::npos) {
            return false;
        }
    }
    return true;
    }
static bool convert_pct(std::string &str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '%')
        {
            if (i + 2 >= str.size() || !std::isxdigit(str[i + 1]) || !std::isxdigit(str[i + 2]))
            {
                return false;
            }
            std::string hex = str.substr(i + 1, 2);
            int c;
            std::stringstream ss;
            ss <<   std::hex << hex;
            ss >>   c;
            str[i] = static_cast<char>(c);
            str.erase(i + 1, 2);
        }
    }
    return true;
}
int read_request(int client_fd, int size,std::string& requestmsg)
{
	int num = 0;
	char buf[size];
	std::string mode;
	num = read(client_fd, buf, size);
	if (num < 1)
		return num;
	requestmsg.append(buf, num);
	// check if the message has completed headers = \r\n\r\n
	if (requestmsg.find("\r\n\r\n") != std::string::npos)
	{
		// if method is POST or PUT, check if the body is complete
		if (requestmsg.find("POST") == 0 || requestmsg.find("PUT") == 0)
		{
			// check that there is a content-length header
			if (requestmsg.find("Content-Length: ") != std::string::npos)
			{
				mode = "Content-Length: ";
				// read the content-length header and convert it to an int
				size_t pos = requestmsg.find(mode);

				size_t end = requestmsg.find("\r\n", pos);
				std::string content_length = requestmsg.substr(pos + mode.length(), end - pos - mode.length());
				unsigned int content_length_int = std::atoi(content_length.c_str());
				//check if the body is complete
				if (requestmsg.length() - requestmsg.find("\r\n\r\n") - 4 < content_length_int)
					return 2;
				else if (requestmsg.length() - requestmsg.find("\r\n\r\n") - 4 > content_length_int)
					{
						// delete message after content-length
						requestmsg = requestmsg.substr(0, requestmsg.find("\r\n\r\n") + 4 + content_length_int);
					}
				return 12;
			}
			else if (requestmsg.find("Transfer-Encoding: chunked\r\n") != std::string::npos)
			{
				mode = "chunked";
				size_t pos = requestmsg.find("\r\n\r\n");
				size_t end = requestmsg.find("\r\n", pos + 4);
				if (pos == std::string::npos)
					return 15;
				if (end == std::string::npos)
					return 2;
				pos += 4;
				std::string chunk_size = requestmsg.substr(pos);
				std::cout << pos <<" "<< end << std::endl;
				std::cout << "chunk_size: {" << chunk_size<< "}" << std::endl;
				int chunk_size_int;
				std::stringstream ss;
				ss <<   std::hex << chunk_size;
				ss >>   chunk_size_int;
				// erase the chunk size line from the message
				if (requestmsg.find("\r\n", pos) == std::string::npos)
					return 14;
				requestmsg.erase(pos, end - pos + 2);
				// delete /r/n after chunk
				requestmsg.erase(requestmsg.length() - 2, 2);
				std::string buffer = buf;
				// count chars after /r/n in buffer
				buffer.erase(0, buffer.find("\r\n")+2);
				int buffer_size = buffer.length();
				if (buffer_size - 2 > chunk_size_int)
					requestmsg.erase(requestmsg.length() - buffer_size - 2 - chunk_size_int, buffer_size - 2 - chunk_size_int);
				std::cout << "chunk_size_int: " << chunk_size_int << std::endl;
				std::cout << "buffer_size: " << buffer_size << std::endl;

				if (chunk_size_int == 0)
					return 13;
				else 
					return 2;
			}
		}
	}
	return 14;
}
void Request::pct_decode()
{
    if ( convert_pct(this->userinfo)== false ||
    convert_pct(this->host)== false ||
    convert_pct(this->port)== false ||
    convert_pct(this->path)== false ||
    convert_pct(this->query)== false ||
    convert_pct(this->fragment)== false)
    {
        this->errcode = 400;
        throw std::runtime_error("Invalid pct encoding");
    }
    //print results
    /* std::cout << BOLD << "~AFTER pct-decoding~ " << std::endl;
    std::cout << BOLD << "userinfo: " <<    this->userinfo << std::endl;
    std::cout << BOLD << "host: " <<        this->host << std::endl;
    std::cout << BOLD << "path: " <<        this->path << std::endl;
    std::cout << BOLD << "query: " <<       this->query << std::endl;
    std::cout << BOLD << "fragment: " <<    this->fragment << std::endl;
    std::cout << BOLD << "port: " <<        this->port << std::endl << std::endl; */
} 


void Request::validate_uri(void)
{
    std::string uri = this->get_uri();

    // get schema
    size_t pos = uri.find(":");
    if (pos != std::string::npos && pos < uri.find("/"))
    {
        std::string schema = uri.substr(0, pos);
        // schema to lower
        for (size_t i = 0; i < schema.size(); i++)
        {
            schema[i] = std::tolower(schema[i]);
        }
        if (schema != "http")
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid schema: " + schema);
        }
        uri = uri.substr(pos + 1);
    }

    // get fragment going from the back of uri
    pos = uri.find("#");
    if (pos != std::string::npos)
    {
        std::string fragment = uri.substr(pos + 1);
        if (!valid_token(fragment, FRAGMENT))
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid fragment: " + fragment);
        }
        this->fragment = fragment;
        uri = uri.substr(0, pos);
    }

    // get query going from the back of uri
    pos = uri.find("?");
    if (pos != std::string::npos)
    {
        std::string query = uri.substr(pos + 1);
        if (!valid_token(query, QUERY))
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid query: " + query);
        }
        this->query = query;
        uri = uri.substr(0, pos);
    }

    // if starts with // then get userinfo, host and port
    if (uri.substr(0, 2) == "//")
    {
        uri = uri.substr(2);
        // get userinfo
        pos = uri.find("@");
        if (pos != std::string::npos)
        {
            std::string userinfo = uri.substr(0, pos);
            if (!valid_token(userinfo, USERINFO))
            {
                this->errcode = 400;
                throw std::runtime_error("Invalid userinfo: " + userinfo);
            }
            this->userinfo = userinfo;
            uri = uri.substr(pos + 1);
        }
        //get path from the back
        pos = uri.find("/");
        if (pos != std::string::npos)
        {
            std::string path = uri.substr(pos);
            if (!valid_token(path, PATH))
            {
                this->errcode = 400;
                throw std::runtime_error("Invalid path: " + path);
            }
            this->path = path;
            uri = uri.substr(0, pos);
        }

        pos = uri.find(":");
        if (pos != std::string::npos)
        {
            std::string port = uri.substr(pos + 1);
            if (!valid_token(port, DIGIT))
            {
                this->errcode = 400;
                throw std::runtime_error("Invalid port: " + port);
            }
            this->port = port;
            uri = uri.substr(0, pos);
        }

        std::string host = uri;
        if (!valid_token(host, HOST))
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid host: " + host);
        }
        this->host = host;
    }
    else {
        std::string path = uri;
        if (!valid_token(path, PATH))
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid path: " + path);
        }
        this->path = path;
    }
    /* std::cout << "~Reading uri~ " << std::endl;
    std::cout << "userinfo: " <<    this->userinfo << std::endl;
    std::cout << "host: " <<        this->host << std::endl;
    std::cout << "path: " <<        this->path << std::endl;
    std::cout << "query: " <<       this->query << std::endl;
    std::cout << "fragment: " <<    this->fragment << std::endl;
    std::cout << "port: " <<        this->port << std::endl << std::endl; */
}

// validate request
void Request::validate_request()
{
    if (this->get_method() != "GET" && this->get_method() != "POST" && this->get_method() != "DELETE" && this->get_method() != "PUT" && this->get_method() != "HEAD" )
    {
        this->errcode = 405;
        throw std::runtime_error("Invalid method: " + this->get_method());
    }
    if (this->get_protocol() != "HTTP/1.1"){
        this->errcode = 505;
        throw std::runtime_error("Invalid protocol: " + this->get_protocol());
    }
    // validate headers - field-names
    for (std::map <std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        if (it->first.size() == 0)
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid header: empty field-name");
        }
        if (!valid_token(it->first, TCHAR))
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid header: invalid field-name");
        }
    }
    // validate headers - field-values - printable ascii characters
    for (std::map <std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        for (size_t i = 0; i < it->second.size(); i++)
        {
            if (!std::isprint(it->second[i]))
            {
                this->errcode = 400;
                throw std::runtime_error("Invalid header: invalid field-value");
            }
        }
    }

}

void Request::check_length()
{
    if (this->get_uri().size() > MAX_URL_LENGTH)
    {
        this->errcode = 414;
        throw std::runtime_error("Request-URI Too Long");
    }
    //headers
    if (this->get_headers().size() > MAX_HEADER_LENGTH)
    {
        this->errcode = 431;
        throw std::runtime_error("Request Header Fields Too Large");
    }

}
void Request::check_headers()
{
    // if no connection header is present, set it to keep-alive
    if (this->headers.find("CONNECTION") == this->headers.end())
    {
        this->headers["CONNECTION"] = "keep-alive";
    }
    // check that host is present
    if (this->headers.find("HOST") == this->headers.end())
    {
        this->errcode = 400;
        throw std::runtime_error("Invalid request: Host header is missing");
    }
    // check that host is not empty
    if (this->headers["HOST"].size() == 0)
    {
        this->errcode = 400;
        throw std::runtime_error("Invalid request: Host header is empty");
    }
    // split into host and port by :
    if (this->headers["HOST"].find(":") != std::string::npos)
    {
        this->host = this->headers["HOST"].substr(0, this->headers["HOST"].find(":"));
        this->port = this->headers["HOST"].substr(this->headers["HOST"].find(":") + 1);
    }
    else
    {
        this->host = this->headers["HOST"];
    }
    // if method is post or put, check that content-length or chuncked is present
    if (this->method == "POST" || this->method == "PUT")
    {
        if (this->headers.find("CONTENT-LENGTH") == this->headers.end() && this->headers.find("TRANSFER-ENCODING") == this->headers.end())
        {
            this->errcode = 411;
            throw std::runtime_error("Invalid request: Content-Length and Transfer-Encoding is missing");
        }
        // if both content-length and transfer-encoding are present, return 400
        if (this->headers.find("CONTENT-LENGTH") != this->headers.end() && this->headers.find("TRANSFER-ENCODING") != this->headers.end())
        {
            this->errcode = 400;
            throw std::runtime_error("Invalid request: Content-Length and Transfer-Encoding are both present");
        }
        // if content-length is present, check that it is a number
        if (this->headers.find("CONTENT-LENGTH") != this->headers.end())
        {
            std::string content_length = this->headers["CONTENT-LENGTH"];
            if (!valid_token(content_length, DIGIT))
            {
                this->errcode = 400;
                throw std::runtime_error("Invalid request: Content-Length is not a number");
            }
        }
    
    }
}

void Request::init(std::string request)
{
    this->errcode = 0;
    parse(request);
    validate_request();
    validate_uri();
    pct_decode();
    check_length();
    check_headers();
    //std::cout << *this << std::endl;
}
void Request::clean(void)
{
    //clean all variables if they exist
    this->errcode = 0;
}
Request::Request(){}
