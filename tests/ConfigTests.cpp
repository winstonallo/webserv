#include "ConfigTests.hpp"
#include "ConfigParser.hpp"
#include <exception>
#include <vector>
#include "Request.hpp"
#include <iomanip>

e_result    print_result(const std::string& message, e_result result)
{
    const int width = 50;

    if (result == FAILURE)
    {
        std::cout << RED;
        std::cout << std::left << std::setw(width) << message << "[KO]" << std::endl;
        return FAILURE;
    }
    else
    {
        std::cout << GREEN;
        std::cout << std::left << std::setw(width) << message << "[OK]"<< std::endl;
        return SUCCESS;
    }
}

void    ParsingTest::test_invalid_files()
{
    for (std::vector<std::string>::iterator it = _test_files.begin(); it != _test_files.end(); it++)
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

ParsingTest::ParsingTest()
{
    _test_files.push_back("/tests/files/empty.conf");
    _test_files.push_back("/tests/files/invalid_extension.con");
    _test_files.push_back("/tests/files/missing_closing_brace.conf");
    _test_files.push_back("/tests/files/no_permission.conf");
    _test_files.push_back("/tests/files/unexpected_nl.conf");
    _test_files.push_back("/tests/files/unterminated_value_scope.conf");
    _test_files.push_back("/tests/files/extra_closing_brace.conf");
    _test_files.push_back("/tests/files/invalid_header.conf");
    _test_files.push_back("/tests/files/missing_opening_brace.conf");
    _test_files.push_back("/tests/files/unclosed_quotes.conf");
    _test_files.push_back("/tests/files/uninitialized_scope.conf");
}

ParsingTest::~ParsingTest() {}