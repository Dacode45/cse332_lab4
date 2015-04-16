/*
json_parser.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Defines functions created in json_parser that are used when loading files to extract the information about the players
*/
#include "stdafx.h"
#include <regex>
#include <map>
#include <string>
#include <vector>
#include "cards.h"
#include <sstream>
#include "json_parser.h"

//Function for loading JSON data
void json_parser::load_json(const std::string& JSON){
		empty = true;

		//Create regex expression
		std::regex object_regex("\\{.+\\}");
		std::smatch object_matches;
		std::regex_match(JSON, object_matches, object_regex);

		if (object_matches.size() == 0){
			throw INVALIDJSONOBJ;
		}
		std::string field = object_matches[0];

		std::regex field_regex("(\\w+\\s*):(\\s*\\w+)");
		std::smatch field_matches;

		//Search for matching fields
		while (std::regex_search(field, field_matches, field_regex)){

			obj.insert(std::make_pair(field_matches[1], field_matches[2]));
			field = field_matches.suffix().str();
		}

		empty = false;
	}

//export as string
std::string json_parser::export_json(){
		std::stringstream ss;
		ss << "{ ";
		for (auto it = obj.begin(); it != obj.end(); it++){
			ss << it->first << ":" << it->second << " ";
		}
		ss << " }";
		return ss.str();
	}

//overloaded export that takes in map of strings to strings
 std::string json_parser::export_json(std::map<std::string, std::string>& o){
		std::stringstream ss;
		ss << "{ ";
		for (auto it = o.begin(); it != o.end(); it++){
			ss << it->first << ":" << it->second << " ";
		}
		ss << " }";
		return ss.str();
	}

//return values given a key
std::string json_parser::get(const std::string& key){
		auto it = obj.find(key);
		if (it != obj.end()){
			return it->second;
		}
		else
			throw JSONKEYNOTFOUND;
	}

//Sets the key and value
void json_parser::set(const std::string& key, const std::string& value){
		obj[key] = value;
	}

//Calls load function
json_parser::json_parser(const std::string& JSON){
		try{
			load_json(JSON);
		}
		catch (int e){
			handleErrMessages(e);
			throw INVALIDJSONOBJ;
		}
	}

