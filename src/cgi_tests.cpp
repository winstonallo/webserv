#include "LocationInfo.hpp"
#include "Utils.hpp"
#include "CGI.hpp"
#include <iostream>
#include <vector>
#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv, char **env)
{   
    try
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
            std::string cgi_script = "files/cgi/a.out";
            CGI cgi = CGI(env_map, locations);
            try
            {
                std::string output = cgi.execute(cgi_script);
            }
            catch (const std::exception& e)
            {
                std::cerr << "error: " << e.what() << " in server " << (*server)->get_server_name()[0] << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }
}