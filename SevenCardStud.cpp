/*
SevenCardStud.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Defines function for playing rounds, including before, during and after. Where method from header are defined
*/
#include "stdafx.h"
#include "Game.h"
#include "SevenCardStud.h"
#include <algorithm>
#include <iostream>

//If both players exist, compare their hands
const unsigned int this_games_ante = 1;
const unsigned int initial_cards = 2;
const unsigned int face_down_cards = 1;
unsigned int current_card = 0;

//Put all the cards in the deck
SevenCardStud::SevenCardStud() :dealer(0), current_bet(0), ante(this_games_ante){

	pot = 0;
	for (int suit = 1; suit <= 4; suit++){
		for (int rank = 1; rank <= 13; rank++){
			Card c = Card(static_cast<Card::SUIT>(suit), static_cast<Card::RANK>(rank));

			main_deck.add_card(c);

		}
	}
	main_deck.shuffle();
}

/*
Plays a round of the game with before_round, round, and after_round all being called
*/
void SevenCardStud::play_round(){

	pot = 0;
	num_players_fold = 0;

	if (int brError = before_round()){

		handleErrMessages(brError);

	}
	if (int rError = round()){

		handleErrMessages(rError);

	}
	if (int arError = after_round()){

		handleErrMessages(arError);

	}
}


void SevenCardStud::betting_phase(player& p){

	if (p.will_fold){
		std::cout << "this player folded " << p.hand;
		return;
	}

	//all other players folded
	if (num_players_fold == players.size() - 1){
		std::cout << "Other players all folded " << p.hand;
		return;
	}

	std::cout << "Player " << p.name << " turn" << std::endl;
	std::cout << "You have " << p.chips << " chips" << std::endl;
	std::cout << "Hand: " << p.hand;

	if (!p.isrobot && !p.will_fold){

		bool player_done = false;

		auto yes_or_no = [](std::string str){
			return (str == "yes" || str == "no");
		};

		//First asks the user if they want to fold
		std::string fold_decision = prompt_string("Would you like to fold", yes_or_no, "Please enter (yes/no)");
		if (!fold_decision.compare("yes")){
			std::cout << "Found a yes " << fold_decision << std::endl;

			p.will_fold = true;
			player_done = true;
			num_players_fold++;
			return;
		}

		std::cout << "has bet " << player_has_bet << std::endl;

		//theres a current bet on table
		//player can either raise, call, or check if you have no money
		if (player_has_bet){

			//we have gone full circle;
			if (highest_better != nullptr && p == *highest_better){
				player_has_bet = false;
				highest_better = nullptr;
			}
			if (player_done)
				return;

			//check if player has enough money
			if (p.chips < current_bet - p.chips_bet){
				add_to_pot(p, p.chips - p.chips_bet);
				std::cout << "You don't have enough money to call, you'll have to go all in" << std::endl;
				player_done = true;
			}
			else if (p.chips == current_bet - p.chips_bet){//only have enough money to call
				add_to_pot(p, p.chips - p.chips_bet);
				std::cout << "You don't have enough money to raise, you'll have to go all in" << std::endl;
				player_done = true;
			}
			else{
				auto bet_or_check = [](std::string str){
					if (str == "call" || str == "raise"){
						return true;
					}
					else
						return false;
				};

				std::string decision = prompt_string("Would you like to call, or raise", bet_or_check, "Please enter (call/raise)");

				if (decision == "call"){
					add_to_pot(p, current_bet - p.chips_bet);
					player_done = true;
				}
				else{
					auto valid_raise = [](int i){
						return(i == 1 || i == 2);
					};
					int raise = prompt_int("How much would you like to raise", valid_raise, "You can only enter 1 or 2");
					current_bet += raise;
					highest_better = &p;
					player_has_bet = true;
					add_to_pot(p, current_bet - p.chips_bet);
					player_done = true;
				}
			}
		}
		else{ // players can bet or check
			std::cout << "Inside else " << fold_decision << std::endl;


			auto bet_or_check = [](std::string str){
				if (str == "check" || str == "bet"){
					return true;
				}
				else
					return false;
			};
			std::string decision = prompt_string("Would you like to check, or bet", bet_or_check, "Please enter (check/bet)");

			if (decision == "check"){
				player_done = true;
			}
			else{
				auto valid_raise = [](int i){
					return(i == 1 || i == 2);
				};
				int raise = prompt_int("How much would you like to bet", valid_raise, "You can only enter 1 or 2");
				current_bet += raise;
				add_to_pot(p, current_bet - p.chips_bet);
				player_done = true;
				highest_better = &p;
				player_has_bet = true;
			}
		}
	}

	std::cout << "You have " << p.chips << " chips\n\tand have bet " << p.chips_bet << std::endl;

}

//Deal players new cards if they discarded
int SevenCardStud::turn(player &p){
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
int SevenCardStud::after_turn(player &p){
	std::cout << std::endl << p.name << " : " << p.hand << std::endl;
	return SUCCESS;
}



//Do deck shuffling and dealing
int SevenCardStud::before_round(){

	pot = 0;
	highest_bet = 0;
	main_deck.shuffle();

	//ante reset folding
	for each (auto p in players)
	{
		p->will_fold = false;
		if (p->chips >= ante){
			add_to_pot(*p, ante);
		}
		else{
			remove_or_reset(*p);
		}
	}

	return SUCCESS;
}

/*
Deals initial cards, calls betting phase
*/
int SevenCardStud::turn_one(){

	size_t start = dealer;
	start = start % players.size();
	int cards_to_deal = players.size() * initial_cards;

	//Deal the face up cards
	do{
		(players[start])->hand << main_deck;
		start = (start + 1) % players.size();
		--cards_to_deal;
	} while (cards_to_deal);
	current_card += initial_cards;

	start = dealer;
	start = start % players.size();
	cards_to_deal = players.size() * face_down_cards;

	//Deal the face down cards
	do{
		(players[start])->hand << main_deck;
		(players[start])->hand.make_down(current_card);
		start = (start + 1) % players.size();
		--cards_to_deal;
	} while (cards_to_deal);
	current_card += initial_cards;

	//betting phase
	int i = 0;
	bool gone_around_once = false;
	while (!gone_around_once && highest_better == nullptr){

		betting_phase(*players[i]);
		if (i + 1 == players.size()){
			std::cout << "gone around once" << std::endl;
			gone_around_once = true;
		}

		i = (i + 1) % players.size();

	}
	return SUCCESS;
}

/*
Deals each player one face down card and does betting
*/
int SevenCardStud::middle_turn(){
	return SUCCESS;
}

/*
Deals each player one face up card and does betting
*/
int SevenCardStud::last_turn(){
	return SUCCESS;
}

//Run turn and afterturn for every player
int SevenCardStud::round(){
	//Call turn one
	if (int turn_error = turn_one()){
		return turn_error;
	}

	//Call middle turn 3 times
	for (int i = 0; i < 3; i++){
		if (int turn_error = middle_turn()){
			return turn_error;
		}
		/*if (int after_turn_error = after_turn()){
			return after_turn_error;
		}*/
	}
	//Call last turn
	if (int turn_error = middle_turn()){
		return turn_error;
	}
	/*if (int after_turn_error = after_turn()){
		return after_turn_error;
	}*/
	return SUCCESS;
}

//Get a winner and print each players hand, let players leave/join, includes auto player functionality
int SevenCardStud::after_round(){

	std::cout << "Current pot is " << pot;

	//only those players who didn't fold can be accessed
	auto strip_folders = [](std::shared_ptr<player> p){
		return (!p->will_fold);
	};
	std::vector<std::shared_ptr<player>> temp_players;
	std::copy_if(players.begin(), players.end(), std::back_inserter(temp_players), strip_folders);

	//Sort the players by hand rank so the winner is first
	std::sort(temp_players.begin(), temp_players.end(), &Game::playerComparator);

	//Give each player their wins and losses and assign probabilities for players in case they are auto
	unsigned int winnerhand;
	std::vector<std::shared_ptr<player>> winners;

	for (auto p = temp_players.begin(); p != temp_players.end(); ++p){
		if (p == temp_players.begin()){
			std::cout << "\nPlayer: " << (*p)->name << " won" << std::endl;
			(*(*p)).games_won++;
			(*p)->my_win_low = player::win_low::win;
			winnerhand = checkHand((*temp_players.begin())->hand.getCards());
			winners.push_back(*p);
		}
		else{
			unsigned int myhand = checkHand((*p)->hand.getCards());
			if (myhand == winnerhand){
				(*(*p)).games_won++;
				(*p)->my_win_low = player::win_low::win;
				winners.push_back(*p);
				std::cout << "\nPlayer: " << (*p)->name << " won" << std::endl;

			}
			else{
				(*(*p)).games_lost++;
				(*p)->my_win_low = player::win_low::ok;

			}
		}

		if (p == temp_players.end() - 1){

			(*p)->my_win_low = player::win_low::low;
		}

		(*(*p)).chips_bet = 0;
		(*(*p)).will_fold = false;

	}

	int num_winners = winners.size();
	int money_per_winner = pot / num_winners;

	for each (std::shared_ptr<player> p in winners)
	{
		p->chips += money_per_winner;
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
