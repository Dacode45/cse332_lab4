/*
card.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Implement methods declared in card.h for loading and adding cards
*/
#include "stdafx.h"
#include "Deck.h"
#include "cards.h"
#include <fstream>
#include <string>
#include <random>
#include <algorithm>
#include <time.h>

//tries to load a file or error
Deck::Deck(const char* &filename){

	int loadErr =
		load(filename);
	
	if (loadErr){
		throw loadErr;
	}
	
}

//loads this file in the same way cards.cpp loads files
int Deck::load(const char* &filename){

	//Which Error to Return
	int toReturn = SUCCESS;
	std::string white_space_buffer; // used for skipping whitespace
	std::ifstream in(filename);
	std::string line;

	//If the file is opened
	if (in.is_open()){

		in >> std::ws;

		int cardsAdded = 0;
		char rankChar, suitChar;

		//While there are still lines
		while (in >> rankChar){

			if (in.fail()){

				in.clear();
				handleErrMessages(program_name, "Error durring File Handling. Will attempt to continue.");
				
			}

			in >> std::ws;
			in >> suitChar;
			
			//Case for 10
			if (suitChar == '0'){

				rankChar = '0';
				in >> std::ws;
				in >> suitChar;

			}

			//If there is a comment move to the next line
			if (suitChar == '/' || rankChar == '/'){// reach end or comment

				continue;

			}
			in >> std::ws;

				Card::RANK r;
				Card::SUIT s;
				int rankErr = getRankFromChar(rankChar, r);
				int suitErr = getSuitFromChar(suitChar, s);
				if (rankErr || suitErr){

					if (!toReturn){

						handleErrMessages(program_name, "Improper File Format");

					}
					continue;

				}
				Card c(s, r);

				this->cards.push_back(c);
				cardsAdded++;

			
		}

		in.close();
		return toReturn;

	}

	else{

		handleErrMessages(program_name, "Couldn't open the file");
		return CANTOPENFILE;

	}

}

//simple suffle based on unsigned random ints
void Deck::shuffle(){

	unsigned int seed = (unsigned int)time(0);
	std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

}

//Gets the size of the deck
const int Deck::size(){

	return cards.size();

}
//Add a card to the deck
void Deck::add_card(Card c){

	this->cards.push_back(c);

}

//Print the deck into an ostream
std::ostream& operator<<(std::ostream& out,const Deck& deck){

	for (auto i = deck.cards.cbegin(); i != deck.cards.cend(); i++){

		out << Card::rankMap[i->rank] << Card::suitMap[i->suit] << std::endl;

	}
	return out;

}

//Collect cards from a hand
void Deck::collectCards(Hand& h){

	std::copy(h.cards.begin(), h.cards.end(), std::back_inserter(this->cards));
	h.cards.erase(h.cards.begin(), h.cards.end());

}

//Collect cards from a deck
void Deck::collectCards(Deck& d){

	std::copy(d.cards.begin(), d.cards.end(), std::back_inserter(this->cards));
	d.cards.erase(d.cards.begin(), d.cards.end());

}