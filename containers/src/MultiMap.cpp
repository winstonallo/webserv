#include "../inc/MultiMap.hpp"
#include <vector>

Value::~Value() {}

StringValue::StringValue(std::string value) : value(value) {}

IntValue::IntValue(int value) : value(value) {}

ListValue::ListValue(std::vector <std::string> values) : values(values) {}

MultiMap::~MultiMap()
{
	for (std::map <std::string, Value*>::iterator it = values.begin(); it != values.end(); it++)
		delete it->second;
}