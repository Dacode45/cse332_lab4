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

/*
Plays a round of the game with before_round, round, and after_round all being called
*/
void FiveCardDraw::play_round(){

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

//Do everything that happens before a turn
int FiveCardDraw::before_turn(player &p){

	if (p.will_fold)
		return SUCCESS;

	//Show the players cards
	std::cout << "Player " << p.name << " turn" << std::endl;
	std::cout << p.name << " : " << p.hand << std::endl;

	size_t num_cards_to_discard = 0;

	//If it's a real player
	if (!p.isrobot){
		bool valid_discards = false;
	
		auto valid_range = [](int i){if (0 <= i && i < 5){ return true; } else return false; };
			num_cards_to_discard = prompt_int("How many cards would you like to discard?\n", valid_range, "You can only enter numbers 0-4");

		
		//Discard however many cards the user wanted to discard
		while (num_cards_to_discard){
			
			std::cout << "Cards:\n" << p.hand << std::endl;
			size_t position = prompt_int("Which card would you like to discard?\n", "");

			//Remove the card from the position specified
			try{
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



void FiveCardDraw::betting_phase(player& p){

	if (p.will_fold){
		std::cout << p.name << " folded " << p.hand << std::endl;
		return;
	}

	//all other players folded
	if (num_players_fold == players.size() - 1){
		std::cout << "Other players all folded " << p.hand << std::endl;
		return;
	}
		
	std::cout << "Player " << p.name << " turn" << std::endl;
	std::cout << "You have " << p.chips << " chips" << std::endl;
	std::cout << "You are obligated to put in " << current_bet - p.chips_bet << " chips." << std::endl;
	std::cout << "Hand: " << p.hand << std::endl;

	if (!p.isrobot && !p.will_fold){

		bool player_done = false;

		auto yes_or_no = [](std::string str){
			str = " " + str;
			return (str.find("yes") != std::string::npos || str.find("no") != std::string::npos);
		};

		//First asks the user if they want to fold
		std::string fold_decision = " " + prompt_string("Would you like to fold", yes_or_no, "Please enter (yes/no)");
		
		if (fold_decision.find("yes") != std::string::npos){
			std::cout << p.name << " decided to fold." << fold_decision.find("yes") << std::endl;

			p.will_fold = true;
			player_done = true;
			num_players_fold++;
		}

		
		//theres a current bet on table
		//player can either raise, call, or check if you have no money
		if (player_has_bet){
			//we have gone full circle;
			if ( p == *highest_better){
				player_has_bet = false;
				highest_better = nullptr;
			}

			if (player_done)
				return;

			//check if player has enough money
			if (p.chips < current_bet - p.chips_bet){
				add_to_pot(p, current_bet - p.chips_bet);
				std::cout << "You don't have enough money to call, you'll have to go all in" << std::endl;
				player_done = true;
			}
			else if (p.chips == current_bet - p.chips_bet){//only have enough money to call
				add_to_pot(p, current_bet - p.chips_bet);
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

				if (decision.find("call") != std::string::npos){
					add_to_pot(p, current_bet - p.chips_bet );
					player_done = true;
				}
				else{
					auto valid_raise = [&p](unsigned int i){
						if (i == 1 || i == 2){
							if (p.chips >= i){
								return true;
							}
						}
						return false;
					};
					add_to_pot(p, current_bet - p.chips_bet);
					int raise = prompt_int("How much would you like to raise", valid_raise, "You can only enter 1 or 2. (Hint: Do you have enough money to put in 2)?");
					current_bet += raise;
					highest_better = &p;
					player_has_bet = true;
					add_to_pot(p, current_bet - p.chips_bet);
					player_done = true;
				}
			}
		}
		else{ // players can bet or check
		
			if (player_done)
				return;

			if (p.chips < 1){
				player_done = true;
				std::cout << "You cant raise you must check" << std::endl;
				return;
			}

			auto bet_or_check = [](std::string str){
				if (str.find("check") != std::string::npos || str.find("bet") != std::string::npos){
					return true;
				}
				else
					return false;
			};
			std::string decision = prompt_string("Would you like to check, or bet", bet_or_check, "Please enter (check/bet)");

			if (decision.find("check") != std::string::npos){
				player_done = true;
			}
			else{
				auto valid_raise = [ &p](unsigned int i){
					if (i == 1 || i == 2){
						if (p.chips >= i){
							return true;
						}
					}
					return false;
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
	
	std::cout << "You have " << p.chips << " chips and have bet " << p.chips_bet << std::endl;

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
	
	return SUCCESS;
}

//Do deck shuffling and dealing
int FiveCardDraw::before_round(){
	
	pot = 0;
	highest_bet = 0;
	main_deck.shuffle();
	size_t start = dealer;
	start = start % players.size();
	int cards_to_deal = players.size() * num_cards_in_hand;

	//ante reset folding
	for each (auto p in players)
	{
		p->will_fold = false;
		num_players_fold = 0;
		//add to ante
		if (p->chips >= ante){
			add_to_pot(*p, ante);
		}
		else{
			remove_or_reset(*p);
		}
	}

	//betting phase
	int i = 0;
	bool gone_around_once = false;
	current_bet = ante;
	while (!gone_around_once || highest_better != nullptr){

		betting_phase(*players[i]);
		if (i + 1 == players.size()){
			gone_around_once = true;
		}

		i = (i + 1) % players.size();

	}

	//Deal to the players one at a time
	do{
		(players[start])->hand << main_deck;
		start = (start + 1) % players.size();
		--cards_to_deal;
	} while (cards_to_deal);

	//Call before turn on every player

	

	//before_turn
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

	//betting phase
	int i = 0;
	bool gone_around_once = false;
	while (!gone_around_once && highest_better == nullptr){

		betting_phase(*players[i]);
		if (i + 1 == players.size()){
			gone_around_once = true;
		}

		i = (i + 1) % players.size();

	}


	return SUCCESS;
}

//Get a winner and print each players hand, let players leave/join, includes auto player functionality
int FiveCardDraw::after_round(){

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

	std::cout << "Total Pot was " << pot << std::endl;

	for each (std::shared_ptr<player> p in winners)
	{
		p->chips += money_per_winner;
	}

	//Prints players hand types
	for (auto p = temp_players.begin(); p != temp_players.end(); ++p){
		std::cout << (*(*p)) << " : " << (*(*p)).hand << "\t" << hand_type((*p)->hand) << std::endl;

		std::cout << (*p)->name << ":\n";
		std::cout << "Has Won: " << (*p)->games_won << " games \n";
		std::cout << "Has Lost: " << (*p)->games_lost << " games \n";
		std::cout << "Has : " << (*p)->chips << " games \n";
		if (!(*p)->will_fold){
			std::cout << "Hand is : " << (*p)->hand << std::endl;
		}
		else{
			std::cout << "Player folded can't show hand\n";
		}


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
	std::cout << "\n current players are " << print_players() << std::endl;
	
	auto yes_or_no = [](std::string str){
		str = " " + str;
		return (str.find("yes") != std::string::npos || str.find("no") != std::string::npos);
	};
	bool players_want_to_leave = true;
	do{
		std::string leave_str = prompt_string("Any players want to leave (yes\no)", yes_or_no, "Enter yes or no ");
		if (leave_str.find("yes") != std::string::npos){
			std::string player_name = prompt_string("What is the player who wants to leave.", "Enter a player name");
			std::shared_ptr<player> p = find_player(player_name.c_str());
			if (p){
				//save player data
				//erase player pointer
				std::cout << "trying to delete player" << std::endl;
				p->save();
				remove_player(p->name.c_str());
			}
		}
		else
			players_want_to_leave = false;

	} while (players_want_to_leave);
	
	
	bool players_want_to_join = false;
	do{
		std::string leave_str = prompt_string("Any players want to join (yes\no)", yes_or_no, "Enter yes or no ");
		if (leave_str.find("yes") != std::string::npos){
			std::string player_name = prompt_string("What is the player who wants to join.", "Enter a player name");
			try{
				add_player(player_name.c_str());
			}
			catch (int e){
				handleErrMessages(e);
			}
		}
		else
			players_want_to_join = false;

	} while (players_want_to_join);

	std::cout << "\n current players are " << print_players() << std::endl;


	return SUCCESS;
}