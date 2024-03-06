#ifndef MULTIMAP_HPP
#define MULTIMAP_HPP
#pragma once

#include "../../inc/Headers.hpp"

class Value
{
	public:

		virtual ~Value();
};

class StringValue : public Value
{
	public: 

		std::string value;
		StringValue(std::string value);
};

class IntValue : public Value
{
	public:

		int value;
		IntValue(int value);
};

class ListValue : public Value
{
	public:

		std::vector <std::string> values;
		ListValue(std::vector <std::string> values);
};

class MultiMap : public Value
{
	public:

		std::map <std::string, Value*> values;
		~MultiMap();
};

#endif