#include "LocationInfo.hpp"
#include "Utils.hpp"
#include "CGI.hpp"
#include <iostream>
#include <vector>
#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv, char **env)
{
    if (argc != 1 || argv == NULL)
    {
        return 1;
    }
    std::map <std::string, std::string> env_map = Utils::get_environment_map(env);
    Config config;
    std::vector <Server *> servers = config.get_servers();
    for (std::vector <Server *>::iterator server = servers.begin(); server != servers.end(); server++)
    {
        std::vector <LocationInfo *> locations = (*server)->get_locations();
        for (std::vector <LocationInfo *>::iterator location = locations.begin(); location != locations.end(); location++)
        {
            if ((*location)->is_cgi() == true)
            {
                CGI cgi = CGI(env_map, "hello", (*location));
            }
        }
    }
}