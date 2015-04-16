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

//If both players exist, compare their hands
bool FiveCardDraw::playerComparator(std::shared_ptr<player> p1, std::shared_ptr<player> p2){
	if (!p1){
		return false;
	}
	else{
		if (!p2){
			return true;
		}
		return poker_rank(p1->hand, p2->hand);
	}

}

//Give the dealer all the cards
FiveCardDraw::FiveCardDraw() :dealer(0){
	for (int suit = 1; suit <= 4; suit++){
		for (int rank = 1; rank <= 13; rank++){
			Card c = Card(static_cast<Card::SUIT>(suit), static_cast<Card::RANK>(rank));

			main_deck.add_card(c);

		}
	}
	main_deck.shuffle();
}

//Do everything that happens before a turn
int FiveCardDraw::before_turn(player &p){

	//Show the players cards
	std::cout << p.name << " : " << p.hand << std::endl;

	bool valid_response = false;
	size_t num_cards_to_discard = 0;

	//If it's a real player
	if (!p.isrobot){

		do{

			//Ask the user for input
			std::cout << "How many cards would you like to discard?\n" << std::endl;
			std::string num_cards;
			std::cin >> num_cards;
			try{

				num_cards_to_discard = stoi(num_cards);
				//Make sure the user gave a valid input
				if (0 <= num_cards_to_discard && num_cards_to_discard < 5){
					valid_response = true;
				}
				else
					throw BADINPUT;

			}

			catch (const std::invalid_argument& ia){
				std::cout << "You must enter a number.\n" << ia.what() << std::endl;
			}
			catch (...){
				std::cout << "Invalid Input\nYou may discard 0-4 cards\n" << std::endl;

			}

		} while (!valid_response);

		valid_response = false;

		//Discard however many cards the user wanted to discard
		while (num_cards_to_discard){
			std::cout << p.name << " : " << p.hand << std::endl;
			std::cout << "Which cards would you like to discard?\n" << std::endl;
			std::string response;
			std::cin >> response;

			//Remove the card from the position specified
			try{
				size_t position = stoi(response);
				Card actual_card = p.hand[position];
				discard_deck.add_card(actual_card);
				p.hand.remove_card(position);
				--num_cards_to_discard;
			}

			//Catch exceptions
			catch (int e){
				switch (e){
				case HANDOUTOFBOUNDS:
					std::cout << "No card at that position\n" << std::endl;
					break;
				default:
					std::cout << "Invalid Input\n" << std::endl;
				}

			}
			catch (const std::invalid_argument& ia){
				std::cout << "You must enter a number.\n" << ia.what() << std::endl;
			}

		}
	}

	//Figure out which cards for the auto player to discard
	else{

		p.get_decision();
		for (auto i = p.card_discard_positions.begin(); i != p.card_discard_positions.end(); ++i){
			Card actual_card = p.hand[*i];
			discard_deck.add_card(actual_card);
			p.hand.remove_card(*i);
		}
		p.clear_decision();

	}
	return SUCCESS;
}

//Do everything required during a turn
int FiveCardDraw::turn(player &p){
	int card_to_deal = num_cards_in_hand - p.hand.size();
	while (card_to_deal != 0){

		//If there are no cards, shuffle discard deck and deal that
		if (main_deck.size() == 0){
			if (discard_deck.size() == 0)
				return NOTENOUGHCARDSTODEAL;
			discard_deck.shuffle();
			p.hand << discard_deck;
		}
		else
			p.hand << main_deck;
		--card_to_deal;
	}
	return SUCCESS;
}

//Print the players hand after the turn
int FiveCardDraw::after_turn(player &p){
	std::cout << std::endl << p.name << " : " << p.hand << std::endl;
	return SUCCESS;
}

//Do deck shuffling and dealing
int FiveCardDraw::before_round(){
	main_deck.shuffle();
	size_t start = dealer;
	start = start % players.size();
	int cards_to_deal = players.size() * num_cards_in_hand;

	//Deal to the players one at a time
	do{
		(players[start])->hand << main_deck;
		start = (start + 1) % players.size();
		--cards_to_deal;
	} while (cards_to_deal);

	//Call before turn on every player
	for (auto p = players.begin(); p != players.end(); p++){
		before_turn(*(*p));
	}

	return SUCCESS;
}

//Run turn and afterturn for every player
int FiveCardDraw::round(){
	for (auto p = players.begin(); p != players.end(); p++){
		if (int turn_error = turn(*(*p))){
			return turn_error;
		}
		if (int after_turn_error = after_turn(*(*p))){
			return after_turn_error;
		}
	}
	return SUCCESS;
}

//Get a winner and print each players hand, let players leave/join, includes auto player functionality
int FiveCardDraw::after_round(){

	std::vector<std::shared_ptr<player>> temp_players(players);

	//Sort the players by hand rank so the winner is first
	std::sort(temp_players.begin(), temp_players.end(), &FiveCardDraw::playerComparator);

	//Give each player their wins and losses and assign probabilities for players in case they are auto
	for (auto p = temp_players.begin(); p != temp_players.end(); ++p){
		if (p == temp_players.begin()){
			std::cout << "\nPlayer: " << (*p)->name << " won" << std::endl;
			(*(*p)).games_won++;
			(*p)->my_win_low = player::win_low::win;
		}
		else{
			(*(*p)).games_lost++;
			(*p)->my_win_low = player::win_low::ok;
		}

		if (p == temp_players.end() - 1){

			(*p)->my_win_low = player::win_low::low;
		}

	}

	//Prints players hand types
	for (auto p = temp_players.begin(); p != temp_players.end(); ++p){
		std::cout << (*(*p)) << " : " << (*(*p)).hand << "\t" << hand_type((*p)->hand) << std::endl;

		main_deck.collectCards((*(*p)).hand);
	}
	main_deck.collectCards(discard_deck);


	std::vector<int> deletion_position;
	//Figure out if the robot players are going to leave the game
	for (int i = players.end() - players.begin() - 1; i >= 0; --i){

		if (players[i]->isrobot){
			int chance = std::rand() % 100;

			switch (players[i]->my_win_low){

			case player::win_low::win:
				if (chance >= 90){

					deletion_position.push_back(i);

				}
				break;

			case player::win_low::ok:
				if (chance >= 50){

					deletion_position.push_back(i);

				}
				break;

			case player::win_low::low:
				if (chance >= 50){

					deletion_position.push_back(i);

				}
				break;

			default:
				break;

			}

		}

	}

	//Save the deleted players and then remove them
	for (auto i = deletion_position.begin(); i != deletion_position.end(); i++){

		players[*i]->save();
		remove_player(players[*i]->name.c_str());

	}


	//See if any players want to leave
	bool players_want_to_leave = true;
	do{

		std::cout << "\n current players are " << print_players() << std::endl;
		std::cout << std::endl;
		std::cout << "Any players want to leave (enter \"y\\n\")\n";
		char answer;
		std::cin >> answer;
		std::cin.ignore();

		if (answer == 'n'){

			players_want_to_leave = false;

		}

		//If they want to leave, find out which person to remove
		else{

			std::cout << "What is the name of the player who wants to leave?\n";
			std::string answer2;
			std::getline(std::cin, answer2);
			std::cin.ignore();
			std::shared_ptr<player> p = find_player(answer2.c_str());

			if (p){

				//save player data
				//erase player pointer
				std::cout << "trying to delete player" << std::endl;
				p->save();
				remove_player(p->name.c_str());

			}

		}

	} while (players_want_to_leave);

	bool players_want_to_join = false;

	//Find out if any players want to join
	do{

		std::cout << "\n current players are " << print_players() << std::endl;
		std::cout << std::endl;
		std::cout << "Any players want to join? (enter \"y\\n\")\n";
		char answer;
		std::cin >> answer;
		std::cin.ignore();

		if (answer == 'n'){

			players_want_to_join = false;

		}

		//Find out who the player is, add them to the game
		else{

			players_want_to_join = true;
			std::cout << "What's the player name?\n";
			std::string name;
			std::getline(std::cin, name);
			std::cin.ignore();

			try{

				add_player(name.c_str());

			}
			catch (int e){

				handleErrMessages(e);
			}

		}

	} while (players_want_to_join);


	return SUCCESS;
}