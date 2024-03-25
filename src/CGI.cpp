#include "CGI.hpp"
#include <cstring>

CGI::CGI(const std::map<std::string, std::string>& env_map, const std::string& body, LocationInfo* location)
{
    _environment_map = env_map;
    _request_body = body;
    _location = location;
    _env = new char*[_environment_map.size() + 1];
    _env[_environment_map.size()] = NULL;
    int i = 0;

    for (std::map <std::string, std::string>::iterator it = _environment_map.begin(); it != _environment_map.end(); it++)
    {
        _env[i++] = strdup((it->first + "=" + it->second).c_str());
    }
}

std::string    CGI::get_cmd()
{
    return ""; // TODO: implement this
}

void    CGI::set_arguments(const std::string& args)
{
    (void)args; // TODO: implement this
}

CGI::~CGI()
{
    for (int i = 0; _env[i]; i++)
    {
        delete _env[i];
    }
    delete[] _env;
}