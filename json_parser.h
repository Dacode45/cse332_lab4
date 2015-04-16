/*
json_parser.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Defines json parser that is used for parsing files for players
*/
#ifndef JSON_PARSER_H
#define JSON_PARSER_H
#include <regex>
#include <map>
#include <string>
#include <vector>
#include "cards.h"
#include <sstream>

struct json_parser{
	bool empty = true;
	std::map<std::string, std::string> obj;


	/*Parses JSON like format
		{
			"field":"value"
			"number":4
		}
	*/
	void load_json(const std::string& JSON);

	std::string export_json();

	static std::string export_json(std::map<std::string, std::string>& o);

	std::string get(const std::string& key);

	void set(const std::string& key, const std::string& value);

	json_parser(const std::string& JSON);
};

#endif