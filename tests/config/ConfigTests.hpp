#ifndef CONFIGTESTS
#define CONFIGTESTS

#include <string>
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

        void    test_invalid_file_structure();

        void    test_invalid_file_value();
        
        void    fill_invalid_file_vector(const std::string& directory, std::vector <std::string>& dest);
    
    private:
        std::vector <std::string> _invalid_structure;
        std::vector <std::string> _invalid_value;
};

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define RED_TEST "\033[1m\033[31m"
#define GREEN_TEST "\033[1m\033[32m"

#endif