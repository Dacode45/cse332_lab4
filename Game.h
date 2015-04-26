/*
Game.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Base class for playing of a generic card game that has methods that are used in FiveCardDraw
*/
#ifndef GAME_H
#define GAME_H

#include <memory>
#include "Deck.h"
#include "player.h"
#include "cards.h"
#include <regex>
#include <functional>

class Game{

//Define protected fields
protected:

	unsigned int pot;
	static std::shared_ptr < Game > game;
	Deck main_deck;
	std::vector<std::shared_ptr<player>> players;
	static bool playerComparator(std::shared_ptr<player> p1, std::shared_ptr<player> p2);
	

//Define public methods for game class
public:
	
	int size();
	
	static std::shared_ptr<Game> instance();

	static void start_game(const char* type);

	static void stop_game();

	void add_player(const char* name);
	std::shared_ptr<player> find_player(const char* name);
	void remove_player(const char* name);
	std::string print_players();

	virtual ~Game();

	virtual void play_round(){}

	virtual int before_turn(player &p){ return SUCCESS; }
	virtual int turn(player &p){ return SUCCESS; }
	virtual int after_turn(player &p){ return SUCCESS; }
	virtual int before_round(){ return SUCCESS; }
	virtual int round(){ return SUCCESS; }
	virtual int after_round(){ return SUCCESS; }

	virtual void remove_or_reset(player& p);

	virtual void add_to_pot(player&, unsigned int amount);

	int isValidNumber(std::string str);
	std::string prompt_string(const char* message, std::function<bool(std::string)> predicate, const char* err_message);
	std::string prompt_string(const char* message, const char* err_message);

	int prompt_int(const char* message, std::function<bool(int)> predicate, const char* err_message);
	int prompt_int(const char* message, const char* err_message);



	//virtual void play_round();

};

#endif