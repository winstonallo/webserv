#include "ConfigDispatcher"

ConfigDispatcher::ConfigDispatcher(std::map <std::string, std::vector <std::string> > raw_config)
{
    if (_raw_config == NULL)
    {
        throw std::runtime_error("cannot instantiate ConfigDispatcher with no raw_config");
    }
    _raw_config = raw_config;
}

ConfigDispatcher::~ConfigDispatcher() {}

ConfigDispatcher::ConfigDispatcher(const ConfigDispatcher& rhs) {*this = rhs}

ConfigDispatcher    &ConfigDispatcher::operator=(const ConfigDispatcher& rhs)
{
    if (*this != rhs)
    {
        _raw_config = rhs._raw_config;
        _servers = rhs._servers;
        _error_pages = rhs._error_pages;
    }
    return *this;
}
