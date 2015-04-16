/*
Deck.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Defines deck which contains a vector of cards and methods to add cards and load them from files
*/
#ifndef DECK_H
#define DECK_H

#include "stdafx.h"
#include "card.h"
#include "cards.h"
#include <vector>
#include <iostream>
#include <random>


class Hand;
class Deck{
	//Insertion operators
	friend Hand& operator<<(Hand& h, Deck& d);
	friend std::ostream& operator<<(std::ostream& out, const Deck& deck);
public:

	Deck() = default;
	Deck(const char* &filename);
	void add_card(Card);
	//using vector dont need a deconstructor
	//compiler supplied is okay
	//vector handles that memory
	int load(const char* &filename);
	void shuffle();
	const int size();
	void collectCards(Hand& h);
	void collectCards(Deck& d);

private:
	Deck(const Deck& d) = delete;
	std::vector<Card> cards;
	
};

std::ostream& operator<<(std::ostream& out, const Deck& deck);


#endif