#include "ConfigTests.hpp"
#include "ConfigParser.hpp"
#include "ConfigDispatcher.hpp"
#include "Config.hpp"
int main(int argc, char **argv)
{
    if (argc == 2)
    {
        ConfigParser        parser(argv[1]);
        ConfigDispatcher    dispatcher(parser.get_config());
        Config              config(dispatcher.get_error_pages());
        std::map <int, std::map <std::string, std::vector <std::string> > > servers = dispatcher.get_servers();
        config.set_servers(servers);
    }
    else
    {
        ParsingTest tester;

        tester.test_invalid_file_structure();
        tester.test_invalid_file_value();
    }
}