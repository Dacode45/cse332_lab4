/*
Hand.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-
Defines the hand class which has a vector of card objects and can take in cards from decks and has operators defined to compare against other hands
*/

#ifndef HAND_H
#define HAND_H

#include "stdafx.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Deck.h"

class Deck;


class Hand{
	friend std::ostream& operator<<(std::ostream& out,const Hand& h);
	friend Hand& operator<<( Hand&h ,  Deck& d);
	friend Deck;
public:
	Hand() = default;
	Hand(const Hand& h) :cards(h.getCards()){};
	//using vector dont need a deconstructor
	//c++ handles that memory

	unsigned int size() const;
	
	//Need all these operators
	Hand& operator=(const Hand& h);
	bool operator==(const Hand& h);
	bool operator<(const Hand& h);
	Card& operator[](const size_t);

	std::string asString() const;

	void remove_card(const size_t);
	const std::vector<Card> getCards() const { return this->cards; } 

private:
		std::vector<Card> cards;

};

std::ostream& operator<<(std::ostream& out ,const Hand& h);



#endif