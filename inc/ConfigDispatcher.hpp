// class responsible for the validation & loading of parsed values into their corresponding object
// eg: ServerInfo, LocationInfo, etc
#ifndef CONFIGDISPATCHER_HPP
#define CONFIGDISPATCHER_HPP

#include <map>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "Utils.hpp"

class ConfigDispatcher
{
    public:

        void                                                                dispatch_values();
        void                                                                handle_error_page(const std::pair <std::string, std::vector <std::string> >& key_value);

        ConfigDispatcher(const std::map <std::string, std::vector <std::string> >& raw_config=std::map<std::string, std::vector<std::string> >());
        ~ConfigDispatcher();

    private:

        std::map <std::string, std::vector <std::string> >                  _raw_config;

        std::map <int, std::map <std::string, std::vector <std::string> > > _servers;
        std::map <int, std::string>                                         _error_pages;

        ConfigDispatcher(const ConfigDispatcher& rhs);
        ConfigDispatcher& operator=(const ConfigDispatcher& rhs);
};

#endif