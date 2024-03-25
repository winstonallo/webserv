#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include "LocationInfo.hpp"

class CGI
{
    public:

        CGI(const std::map<std::string, std::string>& env, const std::string& body, LocationInfo* location);
        ~CGI();
    
    private:

        LocationInfo*       _location;
        std::string         _response_body;
        std::string         _request_body;

};

#endif