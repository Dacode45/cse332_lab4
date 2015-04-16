/*
Hand.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu

*/
#include "stdafx.h"
#include "Hand.h"
#include "card.h"
#include <sstream>

//gets the last card off the deck and then remove it
//put that card in hand
Hand& operator<<(Hand& h, Deck& d){

	h.cards.push_back(d.cards.back());
	d.cards.pop_back();
	std::sort(h.cards.begin(), h.cards.end());
	return h;
}

//deep copy all the items in the hand
Hand& Hand::operator=(const Hand& h){
	this->cards.clear();
	std::copy(h.cards.begin(), h.cards.end(), std::back_inserter(this->cards));
	return *this;
}

//return size of vector
unsigned int Hand::size() const{
	return this->cards.size();
}

//Equivalence method, returns true if all cards are the same
bool Hand::operator==(const Hand& h){
	//check same length
	if (this->size() != h.size()){
		return false;
	}
	auto card1 = this->cards.begin();
	auto card2 = h.cards.begin();
	//check same cards
	while (card1 != this->cards.end()){
		if (!(*card1 == *card2)){
			return false;
		}
		card1++;
		card2++;
	}
	return true;
}

//lexographically order.
//uses heuristic to return smaller hand
bool Hand::operator<(const Hand& h){
	
	//flag smaller. Checks if this hand has fewer cards than the other
	std::vector<Card>::const_iterator smaller;
	std::vector<Card>::const_iterator larger;
	std::vector<Card>::const_iterator end;

	bool thisIsSmaller = this->size() <= h.size();

	if (thisIsSmaller){
		smaller = this->cards.cbegin();
		larger = h.cards.cbegin();
		end = this->cards.cend();
	}
	else{
		larger = this->cards.cbegin();
		smaller = h.cards.cbegin();
		end = h.cards.cend();
	}

	while (smaller != end){
		if (*smaller == *larger){
			//do nothing
		}
		else if (*smaller < *larger){
			if (thisIsSmaller){
				return true;
			}
			else
				return false;
		}
		else{// larger < smaller
			if (thisIsSmaller){
				return false;
			}
			else
				return true;
		}
		smaller++;
	}
	//all cards of the smaller are in the larger and in the same order
	//make sure they aren't equal
	if (this->size() == h.size()){
		return false;
	}
	//not equal return the smaller
	if (thisIsSmaller){
		return true;
	}
	
	return false;
}

//Define the subscript operator
Card& Hand::operator[](const size_t nIndex){
	if (nIndex < this->size() && nIndex >= 0){
		return cards[nIndex];
	}
	else
		throw HANDOUTOFBOUNDS;
}

//Remove a card from the hand given an index
void Hand::remove_card(const size_t index){
	if (index < this->size() && index >= 0){
		this->cards.erase(this->cards.begin() + index);
	}
	else
		throw HANDOUTOFBOUNDS;
}

//print out vaid card definition strings
std::string Hand::asString() const{
	std::stringstream to_return;
	for (auto i = this->cards.begin(); i != cards.end(); i++){
		to_return << Card::rankMap[i->rank] << Card::suitMap[i->suit] << " ";
	}
	return to_return.str();
}

//print to command line
std::ostream& operator<<(std::ostream&out , const Hand& h){
	out << h.asString();
	return out;
}

