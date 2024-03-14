
#include "../inc/Headers.hpp"
#include "../inc/Request.hpp"


Request::~Request()
{
}

void Request::set_protocol(const std::string& val)
{
    this->protocol = val;
}

std::string Request::get_protocol() const
{
    return this->protocol;
}

std::string Request::get_method() const
{
    return this->method;
}

void Request::set_method(const std::string& val)
{
    this->method = val;
}

std::string Request::get_uri() const
{
    return this->uri;
}

void Request::set_uri(const std::string& val)
{
    this->uri = val;
}

std::map <std::string, std::string> Request::get_headers() const
{
    return this->headers;
}

void Request::set_headers(const std::map <std::string, std::string>& val)
{
    this->headers = val;
}

std::string Request::get_body() const
{
    return this->body;
}

void Request::set_body(const std::string& val)
{
    this->body = val;
}


Request::Request(std::string request)
{
    std::string line;
    std::istringstream iss(request);
    std::getline(iss, line);
    std::istringstream iss_line(line);
    std::string word;
    iss_line >> word;
    this->set_method(word);
    iss_line >> word;
    this->set_uri(word);
    iss_line >> word;
    this->set_protocol(word);

    // get headers
    while (std::getline(iss, line))
    {
        if (line == "")
        {
            break;
        }
        std::istringstream iss_line(line);
        std::string key;
        std::string value;
        std::getline(iss_line, key, ':');
        std::getline(iss_line, value);
        this->headers[key] = value;
    }
    // get body
    while (std::getline(iss, line))
    {
        this->body += line + "\n";
    }
    // print request with name in red
    std::cout << RED <<"Method: " << RESET << this->get_method() << std::endl;
    std::cout << RED << "URI: " << RESET << this->get_uri() << std::endl;
    std::cout << RED << "Protocol: " << RESET  << this->get_protocol() << std::endl;
    std::cout << RED << "Headers: " << RESET << std::endl;
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << RED  << "Body: \n" << RESET << this->get_body();

}