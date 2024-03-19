#ifndef CONFIGTESTS
#define CONFIGTESTS

#include "ConfigParser.hpp"
#include <vector>

typedef enum test_outcome
{
    FAILURE,
    SUCCESS
}   e_result;

class ParsingTest
{
    public:
        ParsingTest();
        ~ParsingTest();

        void    test_invalid_files();
    
    private:
        std::vector <std::string> _test_files;
};

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define RED_TEST "\033[1m\033[31m"
#define GREEN_TEST "\033[1m\033[32m"

#endif