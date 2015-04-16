/*
FiveCardDraw.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Game class, contains a deck, methods for taking turns, dealing cards to players, deciding who wins and all other things related to the actual playing of the game
*/
#ifndef FIVECARDDRAW_H
#define FIVECARDDRAW_H

#include "card.h"
#include <iostream>
#include <string>
#include "cards.h"
#include "Game.h"


class Game;
class FiveCardDraw : public Game{

private:
	int num_cards_in_hand = 5;


protected:
	//Protected fields
	size_t dealer;
	Deck main_deck;
	Deck discard_deck;

	//If both players exist, compare their hands
	static bool playerComparator(std::shared_ptr<player> p1, std::shared_ptr<player> p2);

public:

	//Give the dealer all the cards
	FiveCardDraw();

	virtual int before_turn(player &p);

	virtual int turn(player &p);

	virtual int after_turn(player &p);

	virtual int before_round();


	virtual int round();

	virtual int after_round();

};

#endif