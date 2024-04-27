#include <iostream>
#include "Utils.hpp"
#include "Config.hpp"
#include "Request.hpp"

Request::~Request(){}

std::ostream& operator<<(std::ostream& os, Request& req)
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

std::string to_upper(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        str[i] = std::toupper(str[i]);
    }
    return str;
}

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
static std::string dechunk(std::string chunked, int &flag)
{
    std::string dechunked;
    
    size_t pos = 0;
    while (pos < chunked.size())
    {
        size_t end = chunked.find("\r\n", pos);
        if (end == std::string::npos)
            return dechunked;
        std::string hex = chunked.substr(pos, end - pos);
        int size;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> size;
        if (size == 0)
        {
            flag = 1;
            return dechunked;
        }
        pos = end + 2;
        dechunked += chunked.substr(pos, size);
        end = chunked.find("\r\n", pos + size);
        pos = end + 2;
    }
    return dechunked;
}

int Request::read_request(int client_fd, int size,std::string& requestmsg)
{
	int num = 0;
    int first_read = 1;
	char buf[size];
    std::string buff;
	num = read(client_fd, buf, size);
    buff.append(buf, num);
	if (num < 1)
		return num;
    std::string check = requestmsg;
    check.append(buff);
    if (check.find("\r\n\r\n") == std::string::npos)
    {
        requestmsg.append(buff);
        return NOTREAD;
    }
    if (to_upper(check).find("TRANSFER-ENCODING: CHUNKED") != std::string::npos && to_upper(check).find("CONTENT-LENGTH") != std::string::npos)
    {
        requestmsg.append(buff);
        return READ;
    }
    std::string chunked;
    
    if (to_upper(requestmsg).find("TRANSFER-ENCODING: CHUNKED\r\n") != std::string::npos)
        chunked.append(buf, num);
	else
        requestmsg.append(buf, num);
	// check if the message has completed headers = \r\n\r\n
	if (requestmsg.find("\r\n\r\n") != std::string::npos)
	{
		// if method is POST or PUT, check if the body is complete
		if (requestmsg.find("POST") == 0 || requestmsg.find("PUT") == 0)
		{
			// check that there is a CONTENT-LENGTH header
			if (to_upper(requestmsg).find("CONTENT-LENGTH:") != std::string::npos)
			{
				// read the CONTENT-LENGTH header and convert it to an int
				size_t pos = to_upper(requestmsg).find("CONTENT-LENGTH:");

				size_t end = requestmsg.find("\r\n", pos);
				std::string content_length = requestmsg.substr(pos + 15, end - pos - 15);
				unsigned int content_length_int = std::atoi(content_length.c_str());
				//check if the body is complete
				if (requestmsg.length() - requestmsg.find("\r\n\r\n") - 4 < content_length_int)
					return NOTREAD;
				else if (requestmsg.length() - requestmsg.find("\r\n\r\n") - 4 > content_length_int)
                    {
                        // delete message after CONTENT-LENGTH
                        requestmsg = requestmsg.substr(0, requestmsg.find("\r\n\r\n") + 4 + content_length_int);
                    }
				return READ;
			}
			else if (to_upper(requestmsg).find("TRANSFER-ENCODING: CHUNKED\r\n") != std::string::npos)
			{
                std::string chunk;
                int flag = 0;
                size_t pos = requestmsg.find("\r\n\r\n");
                //if not found send 13
                if (pos == std::string::npos)
                    return READ;
                //if \r\n\r\n is not at the end of the message take the chunked part and dechunk it
                if (pos != requestmsg.size() - 4 && first_read == 1)
                {
                    chunk = requestmsg.substr(pos + 4);
                    requestmsg = requestmsg.substr(0, pos + 4);
                    std::string dechunked = dechunk(chunk, flag);
                    requestmsg.append(dechunked);
                    first_read = 0;
                }
                // if chunked is not empty, dechunk it
                if (chunked.size() > 0)
                {
                    std::string dechunked = dechunk(chunked, flag);
                    requestmsg.append(dechunked);
                    chunked.clear();
                }
                if (flag == 1)
                    return READ;
                return NOTREAD;
			}
		}
	}
	return READ;
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

    //read path and count .. so it does not go out of root
    // split path by /
    std::vector<std::string> path_parts;
    std::istringstream iss(this->path);
    std::string part;
    while (std::getline(iss, part, '/'))
    {
        //std::cout <<"{" <<part <<"}" <<std::endl;
        path_parts.push_back(part);
    }
    int count = 0;
    // add +1 if  element is not .. and -1 if it is
    for (size_t i = 0; i < path_parts.size(); i++)
    {
        if (path_parts[i] == "..")
        {
            count--;
        }
        else if (path_parts[i] != "")
        {
            count++;
        }
        if (count < 0)
        {
            this->errcode = 404;
            throw std::runtime_error("Invalid path: .. goes out of root");
        }
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
        if (!valid_token(it->first, TCHAR) )
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
    // check that sum of all headers is not greater than 8kb
    int count = 0;
    for (std::map <std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        count += it->first.size() + it->second.size();
    }
    if (count > MAX_HEADER_LENGTH)
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
    if (this->headers["HOST"].find(":") != std::string::npos)
    {
        this->host = this->headers["HOST"].substr(0, this->headers["HOST"].find(":"));
        this->port = this->headers["HOST"].substr(this->headers["HOST"].find(":") + 1);
    }
    else
    {
        this->host = this->headers["HOST"];
    }
    // if method is post or put, check that CONTENT-LENGTH or chuncked is present
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
    this->method.clear();
    this->uri.clear();
    this->protocol.clear();
    this->headers.clear();
    this->body.clear();
    this->host.clear();
    this->port.clear();
    this->path.clear();
    this->query.clear();
    this->fragment.clear();
    this->userinfo.clear();
    this->errcode = 0;
}
Request::Request(){

}
