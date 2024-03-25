#include "CGI.hpp"
#include <cstring>
#include <stdexcept>
#include <iostream>

CGI::CGI(const std::map<std::string, std::string>& env, const std::string& body, LocationInfo* location)
{
    _env_map = env;
    _request_body = body;
    _location = location;
    _env = new char*[_env_map.size() + 1];
    _env[_env_map.size()] = NULL;
    int i = 0;
    for (std::map <std::string, std::string>::iterator it = _env_map.begin(); it != _env_map.end(); it++)
    {
        _env[i++] = strdup((it->first + "=" + it->second).c_str());
    }
}

CGI::~CGI() 
{
    for (int i = 0; _env[i]; i++)
    {
        delete _env[i];
    }
    delete[] _env;
}