#include "ConfigTests.hpp"
#include "ConfigParser.hpp"
#include "ConfigDispatcher.hpp"
#include "Config.hpp"
int main(int argc, char **argv)
{
    if (argc == 2)
    {
        try
        {
            ConfigParser        parser(argv[1]);
            ConfigDispatcher    dispatcher(parser.get_config());
            Config              config(dispatcher.get_error_pages());
            std::map <int, std::map <std::string, std::vector <std::string> > > servers = dispatcher.get_servers();
            config.set_servers(servers);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
    else
    {
        ParsingTest tester;

        tester.test_invalid_file_structure();
        tester.test_invalid_file_value();
    }
}