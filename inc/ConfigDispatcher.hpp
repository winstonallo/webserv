// class responsible for the validation & loading of parsed values into their corresponding object
// eg: ServerInfo, LocationInfo, etc
#ifndef CONFIGDISPATCHER_HPP
#define CONFIGDISPATCHER_HPP

#include <map>
#include <iostream>
#include <vector>

class ConfigDispatcher
{
    public:

        ConfigDispatcher(std::map <std::string, std::vector <std::string> > raw_config=NULL);
        ~ConfigDispatcher();

    private:

        std::map <std::string, std::vector <std::string> >                  _raw_config;

        std::map <int, std::map <std::string, std::vector <std::string> >   _servers;
        std::map <int, std::string>                                         _error_pages;

        ConfigDispatcher(const ConfigDispatcher& rhs);
        ConfigDispatcher& operator=(const ConfigDispatcher& rhs);
};

#endif