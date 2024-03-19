#include "ConfigTests.hpp"

int main()
{
    ParsingTest tester;

    tester.test_invalid_file_structure();
    tester.test_invalid_file_value();
}