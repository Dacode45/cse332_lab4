/*
player.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Keeps track of name, wins, losses, Hand, and method for save and comparing
*/
#include "stdafx.h"
#include "card.h"
#include "Hand.h"
#include "Deck.h"
#include "cards.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "json_parser.h"

//Player struct
struct player{
	std::string name;
	bool isrobot = false;
	Hand hand;
	unsigned int games_won;
	unsigned int games_lost;

	enum win_low{win,ok,low};
	win_low my_win_low = ok;

	std::vector<int> card_discard_positions;


	player(std::string n);
	~player();
	void save();

	void get_decision();
	void clear_decision();

	bool operator==(player & p);
};

std::ostream& operator<<(std::ostream& out, const player& p);