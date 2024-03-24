#include "ConfigTests.hpp"
#include "Config.hpp"
#include <exception>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "Request.hpp"
#include <iomanip>
#include <dirent.h>
#include <fstream>

e_result    print_result(const std::string& message, e_result result)
{
    const int width = 70;

    if (result == FAILURE)
    {
        std::cout << std::left << std::setw(width) << message << RED << "[KO]" << std::endl;
        std::cout << RESET;
        return FAILURE;
    }
    else
    {
        std::cout << std::left << std::setw(width) << message << GREEN << "[OK]"<< std::endl;
        std::cout << RESET;
        return SUCCESS;
    }
}

void    ParsingTest::test_invalid_file_structure()
{
    for (std::vector<std::string>::iterator it = _invalid_structure.begin(); it != _invalid_structure.end(); it++)
    {
        try 
        {
            Config config(*it);
            print_result(*it + BOLD + ": failed to throw exception" + RESET, FAILURE);
        }
        catch (const std::exception& e) 
        {
            print_result(*it + BOLD + ": threw exception" + RESET, SUCCESS);
        }
    }
}

void    ParsingTest::test_invalid_file_value()
{
    for (std::vector <std::string>::iterator it = _invalid_value.begin(); it != _invalid_value.end(); it++)
    {
        try
        {
            std::streambuf* old_cerr = std::cerr.rdbuf();
            std::ofstream file_stream("err.txt");
            std::cerr.rdbuf(file_stream.rdbuf());

            try
            {
<<<<<<< HEAD
                Config              config(*it);
=======
                ConfigParser        parser(*it);
                ConfigDispatcher    dispatcher(parser.get_config());
                Config              config(dispatcher.get_error_pages());
                std::map <int, std::map <std::string, std::vector <std::string> > > servers = dispatcher.get_servers();
                config->set_servers(servers);
>>>>>>> origin/newServer
            }
            catch (const std::exception& e)
            {
                print_result(*it + BOLD + ": threw exception" + RESET, SUCCESS);
                std::cerr.rdbuf(old_cerr);
                throw ;
            }

            std::ifstream in_file("err.txt");
            bool was_written_to = in_file.peek() != std::ifstream::traits_type::eof();
            if (was_written_to == true)
            {
                print_result(*it + BOLD + ": wrote to error file" + RESET, SUCCESS);
            }
            else
            {
                print_result(*it + BOLD + ": no error log/exception" + RESET, FAILURE);
            }
        }
        catch (const std::exception& e)
        {
            continue ;
        }
    }
    unlink("err.txt");
}


void    ParsingTest::fill_invalid_file_vector(const std::string& directory, std::vector <std::string>& dest)
{
    DIR*            dir;
    struct dirent*  ent;

    if ((dir = opendir(directory.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string directory_name = ent->d_name;

            if (directory_name != "." && directory_name != "..")
            {
                dest.push_back(ent->d_name);
            }
        }
        closedir(dir);
    }
}

ParsingTest::ParsingTest()
{
    fill_invalid_file_vector("config_tests/invalid_config_structure", _invalid_structure);
    fill_invalid_file_vector("config_tests/invalid_config_value", _invalid_value);
}

ParsingTest::~ParsingTest() {}