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

class Game{

//Define protected fields
protected:

	static std::shared_ptr < Game > game;
	Deck main_deck;
	std::vector<std::shared_ptr<player>> players;

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

	virtual int before_turn(player &p){ return SUCCESS; }
	virtual int turn(player &p){ return SUCCESS; }
	virtual int after_turn(player &p){ return SUCCESS; }
	virtual int before_round(){ return SUCCESS; }
	virtual int round(){ return SUCCESS; }
	virtual int after_round(){ return SUCCESS; }

};

#endif