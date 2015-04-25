/*
FiveCardDraw.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Defines function for playing rounds, including before, during and after. This is where most of the game is played
*/
#include "stdafx.h"
#include "Game.h"
#include "FiveCardDraw.h"
#include <algorithm>
#include <iostream>

//If both players exist, compare their hands
const unsigned int this_games_ante = 1;



//Give the dealer all the cards
FiveCardDraw::FiveCardDraw() :dealer(0), current_bet(0), ante(this_games_ante){

	pot = 0;
	for (int suit = 1; suit <= 4; suit++){
		for (int rank = 1; rank <= 13; rank++){
			Card c = Card(static_cast<Card::SUIT>(suit), static_cast<Card::RANK>(rank));

			main_deck.add_card(c);

		}
	}
	main_deck.shuffle();
}