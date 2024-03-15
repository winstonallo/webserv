
#include "Request.hpp"


Request::~Request()
{
}

std::ostream& operator<<(std::ostream& os, const Request& req)
{
    // print request with name in red
    os << RED <<"Method: " << RESET << req.get_method() << std::endl;
    os << RED << "URI: " << RESET << req.get_uri() << std::endl;
    os << RED << "Protocol: " << RESET  << req.get_protocol() << std::endl;
    os << RED << "Headers: " << RESET << std::endl;
    std::map <std::string, std::string> headers = req.get_headers();
    for (std::map <std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        os << "\t" << it->first << ": " << it->second << std::endl;
    }
    os << RED  << "Body: \n" << RESET;
    std::stringstream ss(req.get_body());
    std::string line;
    while (std::getline(ss, line)) {
        std::cout << "\t" << line << std::endl;
    }
    return os;
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

std::string Request::get_header(const std::string& key) const
{
    if (this->headers.find(key) != this->headers.end())
    {
        return this->headers.at(key);
    }
    return NULL;
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
        std::cout << line << std::endl;
        std::cout << std::count(line.begin(), line.end(), ' ') << std::endl;
        throw std::runtime_error("Invalid request");
    }
    //check that there are now white spaces in the line except for the 2 spaces
    for (size_t i = 0; i < line.size() - 2; i++)
    {
        if (std::isspace(line[i]) != 0 && line[i] != ' ')
        {
            std::cout << i << std::endl;
            std::cout << (int)line[i] << std::endl;
            throw std::runtime_error("Invalid request");
        }
    }

    //check that line ends with \r\n
    if (line[line.size() - 1] != '\r')
    {
        throw std::runtime_error("Invalid request");
    }
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
            throw std::runtime_error("Invalid request");
        }
        //key to upper
        for (size_t i = 0; i < key.size(); i++)
        {
            key[i] = std::toupper(key[i]);
        }
        if (this->headers.find(key) != this->headers.end())
        {
            std::cout << "key: " << key << std::endl;
            std::cout << "value: " << value << std::endl;
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
// validate request

void Request::validate()
{
    if (this->get_method() != "GET" && this->get_method() != "POST" && this->get_method() != "DELETE")
    {
        throw std::runtime_error("Invalid method");
    }
    if (this->get_protocol() != "HTTP/1.1"){
        throw std::runtime_error("Invalid protocol");
    }
    // go through headers and validate for printable ascii characters
    for (std::map <std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        if (it->first.size() == 0)
        {
            throw std::runtime_error("Invalid header2");
        }
        for (size_t i = 0; i < it->first.size() - 1; i++)
        {
            if ((std::isprint(it->first[i]) == 0) || std::isspace(it->first[i]) != 0 )
            {
                std::cout << it->first << std::endl;
                std::cout << (int)it->first[i] << std::endl;
                throw std::runtime_error("Invalid header1");
            }
        }
    }

}
// constructor
Request::Request(std::string request)
{
    parse(request);
    validate();
    

}