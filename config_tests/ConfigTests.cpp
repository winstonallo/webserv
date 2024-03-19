#include "ConfigTests.hpp"
#include "ConfigParser.hpp"
#include "ConfigDispatcher.hpp"
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
    const int width = 50;

    if (result == FAILURE)
    {
        std::cout << RED;
        std::cout << std::left << std::setw(width) << message << "[KO]" << std::endl;
        std::cout << RESET;
        return FAILURE;
    }
    else
    {
        std::cout << GREEN;
        std::cout << std::left << std::setw(width) << message << "[OK]"<< std::endl;
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
            ConfigParser parser(*it);
            print_result(*it, FAILURE);
        }
        catch (const std::exception& e) 
        {
            print_result(*it, SUCCESS);
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
                // Your code here
                ConfigParser        parser(*it);
                ConfigDispatcher    dispatcher(parser.get_config());
                Config              config(dispatcher.get_error_pages());
                std::map <int, std::map <std::string, std::vector <std::string> > > servers = dispatcher.get_servers();
                config.set_servers(servers);
            }
            catch (const std::exception& e)
            {
                print_result(*it, SUCCESS);
                std::cerr.rdbuf(old_cerr);
                throw;  // Re-throw the exception to be caught by the outer catch block
            }

            // Check if anything was written to the file
            std::ifstream inFile("err.txt");
            bool was_written_to = inFile.peek() != std::ifstream::traits_type::eof();
            if (was_written_to == true)
            {
                print_result(*it, SUCCESS);
            }
            else
            {
                print_result(*it, FAILURE);
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
    fill_invalid_file_vector("config_tests/invalid_config_structure", _invalid_value);
    fill_invalid_file_vector("config_tests/invalid_config_value", _invalid_structure);
}

ParsingTest::~ParsingTest() {}