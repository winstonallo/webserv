#include <iostream>
#include "Utils.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include <sstream>
#include <algorithm>
#include "Utils.hpp"

std::string Request::get_header(const std::string& key) const
{
    if (_headers.find(key) != _headers.end())
    {
    	return _headers.at(key);
    }
    return "default";
}

std::string to_upper(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        str[i] = std::toupper(str[i]);
    }
    return str;
}

void Request::parse(std::string request)
{
    std::string line;
    std::istringstream iss(request);
    std::getline(iss, line);
    if (std::count(line.begin(), line.end(), ' ') != 2)
    {
        _errcode = 400;
        throw std::runtime_error("Invalid request: line does not contain 2 spaces between method, uri and protocol");
    }
    for (size_t i = 0; i < line.size() - 2; i++)
    {
        if (std::isspace(line[i]) != 0 && line[i] != ' ')
        {
            _errcode = 400;
            throw std::runtime_error("Invalid request: line contains white spaces except for the 2 spaces between method, uri and protocol");
        }
    }

    if (line[line.size() - 1] != '\r')
    {
        _errcode = 400;
        throw std::runtime_error("Invalid request: line does not end with \\r");
    }
    std::istringstream iss_line(line);
    std::string method;
    std::string uri;
    std::string protocol;
    iss_line >> method;
    iss_line >> uri;
    iss_line >> protocol;
    if (method.size() == 0 || uri.size() == 0 || protocol.size() == 0)
    {
        _errcode = 400;
        throw std::runtime_error("Invalid request: method, uri or protocol is empty");
    }
    _method = method;
    _uri = uri;
    _protocol = protocol;

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
            _errcode = 400;
            throw std::runtime_error("Invalid request: header does not end with \\r");
        }
        for (size_t i = 0; i < key.size(); i++)
        {
            key[i] = std::toupper(key[i]);
        }
        if (_headers.find(key) != _headers.end())
        {
            value = _headers[key] + "," + Utils::trim(value, " \t\n\r");
            _headers[key] = value;
        }
        else{
            _headers[key] = Utils::trim(value, " \t\n\r");}
        
    }
    while (std::getline(iss, line))
    {
        body += line + "\n";
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
        if (valid_chars.find(c) == std::string::npos)
        {
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
        {
            return dechunked;
        }
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
    {
		return num;
    }
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
    {
        chunked.append(buf, num);
    }
    else
    {
        requestmsg.append(buf, num);
    }
	if (requestmsg.find("\r\n\r\n") != std::string::npos)
	{
		if (requestmsg.find("POST") == 0 || requestmsg.find("PUT") == 0)
		{
			if (to_upper(requestmsg).find("CONTENT-LENGTH:") != std::string::npos)
			{
				size_t pos = to_upper(requestmsg).find("CONTENT-LENGTH:");
				size_t end = requestmsg.find("\r\n", pos);
				std::string content_length = requestmsg.substr(pos + 15, end - pos - 15);
				unsigned int content_length_int = std::atoi(content_length.c_str());

				if (requestmsg.length() - requestmsg.find("\r\n\r\n") - 4 < content_length_int)
                {
					return NOTREAD;
                }
				else if (requestmsg.length() - requestmsg.find("\r\n\r\n") - 4 > content_length_int)
                {
                    requestmsg = requestmsg.substr(0, requestmsg.find("\r\n\r\n") + 4 + content_length_int);
                }
				return READ;
			}
			else if (to_upper(requestmsg).find("TRANSFER-ENCODING: CHUNKED\r\n") != std::string::npos)
			{
                std::string chunk;
                int flag = 0;
                size_t pos = requestmsg.find("\r\n\r\n");
                if (pos == std::string::npos)
                {
                    return READ;
                }
                if (pos != requestmsg.size() - 4 && first_read == 1)
                {
                    chunk = requestmsg.substr(pos + 4);
                    requestmsg = requestmsg.substr(0, pos + 4);
                    std::string dechunked = dechunk(chunk, flag);
                    requestmsg.append(dechunked);
                    first_read = 0;
                }
                if (chunked.size() > 0)
                {
                    std::string dechunked = dechunk(chunked, flag);
                    requestmsg.append(dechunked);
                    chunked.clear();
                }
                if (flag == 1)
                {
                    return READ;
                }
                return NOTREAD;
			}
		}
	}
	return READ;
}

void Request::pct_decode()
{
    if ( convert_pct(_userinfo)== false ||
    convert_pct(_host)== false ||
    convert_pct(_port)== false ||
    convert_pct(_path)== false ||
    convert_pct(_query)== false ||
    convert_pct(_fragment)== false)
    {
        _errcode = 400;
        throw std::runtime_error("Invalid pct encoding");
    }
} 

void Request::validate_uri(void)
{
    std::string uri = get_uri();
    size_t pos = uri.find(":");

    if (pos != std::string::npos && pos < uri.find("/"))
    {
        std::string schema = uri.substr(0, pos);
        for (size_t i = 0; i < schema.size(); i++)
        {
            schema[i] = std::tolower(schema[i]);
        }
        if (schema != "http")
        {
            _errcode = 400;
            throw std::runtime_error("Invalid schema: " + schema);
        }
        uri = uri.substr(pos + 1);
    }

    pos = uri.find("#");
    if (pos != std::string::npos)
    {
        std::string fragment = uri.substr(pos + 1);
        if (!valid_token(fragment, FRAGMENT))
        {
            _errcode = 400;
            throw std::runtime_error("Invalid fragment: " + fragment);
        }
        _fragment = fragment;
        uri = uri.substr(0, pos);
    }

    pos = uri.find("?");
    if (pos != std::string::npos)
    {
        std::string query = uri.substr(pos + 1);
        if (!valid_token(query, QUERY))
        {
            _errcode = 400;
            throw std::runtime_error("Invalid query: " + query);
        }
        _query = query;
        uri = uri.substr(0, pos);
    }

    if (uri.substr(0, 2) == "//")
    {
        uri = uri.substr(2);
        pos = uri.find("@");
        if (pos != std::string::npos)
        {
            std::string userinfo = uri.substr(0, pos);
            if (!valid_token(userinfo, USERINFO))
            {
                _errcode = 400;
                throw std::runtime_error("Invalid userinfo: " + userinfo);
            }
            _userinfo = userinfo;
            uri = uri.substr(pos + 1);
        }
        pos = uri.find("/");
        if (pos != std::string::npos)
        {
            std::string path = uri.substr(pos);
            if (!valid_token(path, PATH))
            {
                _errcode = 400;
                throw std::runtime_error("Invalid path: " + path);
            }
            _path = path;
            uri = uri.substr(0, pos);
        }

        pos = uri.find(":");
        if (pos != std::string::npos)
        {
            std::string port = uri.substr(pos + 1);
            if (!valid_token(port, DIGIT))
            {
                _errcode = 400;
                throw std::runtime_error("Invalid port: " + port);
            }
            _port = port;
            uri = uri.substr(0, pos);
        }

        std::string host = uri;
        if (!valid_token(host, HOST))
        {
            _errcode = 400;
            throw std::runtime_error("Invalid host: " + host);
        }
        _host = host;
    }
    else 
    {
        std::string path = uri;
        if (!valid_token(path, PATH))
        {
            _errcode = 400;
            throw std::runtime_error("Invalid path: " + path);
        }
        _path = path;
    }
    std::vector<std::string> path_parts;
    std::istringstream iss(_path);
    std::string part;
    while (std::getline(iss, part, '/'))
    {
        path_parts.push_back(part);
    }
    int count = 0;
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
            _errcode = 404;
            throw std::runtime_error("Error: Invalid path in request: '..' goes out of root");
        }
    }
}

// validate request
void Request::validate_request()
{
    if (get_method() != "GET" && get_method() != "POST" && get_method() != "DELETE" && get_method() != "PUT" && get_method() != "HEAD" )
    {
        _errcode = 405;
        throw std::runtime_error("Invalid method: " + get_method());
    }
    if (get_protocol() != "HTTP/1.1")
    {
        _errcode = 505;
        throw std::runtime_error("Invalid protocol: " + get_protocol());
    }
    // validate headers - field-names
    for (std::map <std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        if (it->first.size() == 0)
        {
            _errcode = 400;
            throw std::runtime_error("Invalid header: empty field-name");
        }
        if (!valid_token(it->first, TCHAR) )
        {
            _errcode = 400;
            throw std::runtime_error("Invalid header: invalid field-name");
        }
    }
    // validate headers - field-values - printable ascii characters
    for (std::map <std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        for (size_t i = 0; i < it->second.size(); i++)
        {
            if (!std::isprint(it->second[i]))
            {
                _errcode = 400;
                throw std::runtime_error("Invalid header: invalid field-value");
            }
        }
    }

}

void Request::check_length()
{
    if (get_uri().size() > MAX_URL_LENGTH)
    {
        _errcode = 414;
        throw std::runtime_error("Request-URI Too Long");
    }
    // check that sum of all headers is not greater than 8kb
    int count = 0;
    for (std::map <std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        count += it->first.size() + it->second.size();
    }
    if (count > MAX_HEADER_LENGTH)
    {
        _errcode = 431;
        throw std::runtime_error("Request Header Fields Too Large");
    }

}
void Request::check_headers()
{
    // if no connection header is present, set it to keep-alive
    if (_headers.find("CONNECTION") == _headers.end())
    {
        _headers["CONNECTION"] = "keep-alive";
    }
    if (_headers.find("HOST") == _headers.end())
    {
        std::cout << "host header is missing" << std::endl;
        _errcode = 400;
        throw std::runtime_error("Invalid request: Host header is missing");
    }
    // check that host is not empty
    if (_headers["HOST"].size() == 0)
    {
        std::cout << "host header is empty" << std::endl;
        _errcode = 400;
        throw std::runtime_error("Invalid request: Host header is empty");
    }
    if (_headers["HOST"].find(":") != std::string::npos)
    {
        _host = _headers["HOST"].substr(0, _headers["HOST"].find(":"));
        _port = _headers["HOST"].substr(_headers["HOST"].find(":") + 1);
    }
    else
    {
        _host = _headers["HOST"];
    }
    if (_method == "POST" || _method == "PUT" || _method == "GET")
    {
        if (_method != "GET" && _headers.find("CONTENT-LENGTH") == _headers.end() && _headers.find("TRANSFER-ENCODING") == _headers.end())
        {
            _errcode = 411;
            throw std::runtime_error("Invalid request: Content-Length and Transfer-Encoding is missing");
        }
        if (_method != "GET" && _headers.find("CONTENT-LENGTH") != _headers.end() && _headers.find("TRANSFER-ENCODING") != _headers.end())
        {
            _errcode = 400;
            throw std::runtime_error("Invalid request: Content-Length and Transfer-Encoding are both present");
        }
        if (_headers.find("CONTENT-LENGTH") != _headers.end())
        {
            std::string content_length = _headers["CONTENT-LENGTH"];
            if (!valid_token(content_length, DIGIT) || std::atoi(content_length.c_str()) < 0)
            {
                _errcode = 400;
                throw std::runtime_error("Invalid request: Content-Length is not a positive number");
            }
        }
    
    }
}

void Request::init(std::string request)
{
    _errcode = 0;
    parse(request);
    validate_request();
    validate_uri();
    pct_decode();
    check_length();
    check_headers();
}
void Request::clean(void)
{
    _method.clear();
    _uri.clear();
    _protocol.clear();
    _headers.clear();
    body.clear();
    _host.clear();
    _port.clear();
    _path.clear();
    _query.clear();
    _fragment.clear();
    _userinfo.clear();
    _errcode = 0;
}
