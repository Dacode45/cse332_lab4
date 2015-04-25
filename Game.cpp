/*
card.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Defines methods declared in game.cpp for playing a generic game
*/
#include "stdafx.h"
#include "Game.h"
#include "FiveCardDraw.h"
#include "SevenCardStud.h"


std::shared_ptr<Game> Game::game;


Game::Game() :dealer(0), current_bet(0), ante(0){

	pot = 0;
	for (int suit = 1; suit <= 4; suit++){
		for (int rank = 1; rank <= 13; rank++){
			Card c = Card(static_cast<Card::SUIT>(suit), static_cast<Card::RANK>(rank));

			main_deck.add_card(c);

		}
	}
	main_deck.shuffle();
}

//Return number of players
int Game::size(){

	return players.size();

}

//Return each players name as a string
std::string Game::print_players(){

	std::string toReturn = "";
	for (auto p = players.begin(); p != players.end(); ++p){
		toReturn += (*p)->name + ",\t";
	}
	return toReturn;

}

/*
Plays a round of the game with before_round, round, and after_round all being called
*/
void Game::play_round(){

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
int Game::before_turn(player &p){

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


/*
Asks a player about betting
Always asks if they wany to fold
If no one bet before asks for check or bet
If someone has best, ask for raise, or call
*/
void Game::betting_phase(player& p){

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


/*
Deals a players hand based on how many cards they discarded
*/
int Game::turn(player &p){
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
int Game::after_turn(player &p){
	std::cout << std::endl << p.name << " : " << p.hand << std::endl;
	return SUCCESS;
}



//Do deck shuffling and dealing
int Game::before_round(){

	pot = 0;
	highest_bet = 0;
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

	//ante reset folding
	for each (auto p in players)
	{
		p->will_fold = false;
		if (p->chips > 0){
			add_to_pot(*p, ante);
		}
		else{
			remove_or_reset(*p);
		}
	}

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

	//before_turn
	for (auto p = players.begin(); p != players.end(); p++){

		before_turn(*(*p));
	}

	return SUCCESS;
}

//Run turn and afterturn for every player
int Game::round(){
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
int Game::after_round(){

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

/*
Compares two players by the poker rank of their hands
*/
bool Game::playerComparator(std::shared_ptr<player> p1, std::shared_ptr<player> p2){
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


/*
Asks a player if they want more chips
If they do call reset, else remove them. Auto removes robots
*/
void Game::remove_or_reset(player&p){
	if (!p.isrobot){
		std::cout << "Hi. You're out of money friend. Would you like some more (yes/no)" << std::endl;
		std::string more_money;
		std::cin >> more_money;
		if (more_money.find("no")){
			std::cout << "Goodbye then \n";
			remove_player(p.name.c_str());
		}
		else
			p.reset();
	}
	else
		remove_player(p.name.c_str());
}


//Makes a shared pointer instance
 std::shared_ptr<Game> Game::instance(){
	if (!game)
		throw instance_not_available;
	std::shared_ptr<Game> g_copy(game);
	return g_copy;
}

 //Allow user to specify game type but if its not fivecarddraw throw error
 void Game::start_game(const char* type){
	if (game)
		throw game_already_started;

	std::string game_type = type;
	if (game_type == "FiveCardDraw"){
		game = std::static_pointer_cast<Game>(std::make_shared<FiveCardDraw>());

	}
	else if (game_type == "SevenCardStud"){
		//game = std::static_pointer_cast<Game>(std::make_shared<SevenCardStud>());
	}
	else{
		throw unknown_game;
	}
		

}

//Reset the game
void Game::stop_game(){
	if (game){
		game.reset();
	}
	else
		throw no_game_in_progress;

}

//Add a player to the game given a name
void Game::add_player(const char* name){
	auto duplicate_player = find_player(name);
	if (duplicate_player){
		//throw exception
		throw already_playing;
	}
	else{
		players.push_back(std::make_shared<player>(name));
	}
}

//Find a player from the game
 std::shared_ptr<player> Game::find_player(const char* name){
	for (std::vector<std::shared_ptr<player>>::iterator p = players.begin(); p != players.end(); ++p){
		if ((*p)->name == name){
			return *p;
		}
	}
	std::shared_ptr<player> failed;
	return failed;
}

 //Remove a player from the game
 void Game::remove_player(const char* name){
	 for (std::vector<std::shared_ptr<player>>::iterator p = players.begin(); p != players.end(); ++p){
		 if ((*p)->name == name){
			 players.erase(p);
			 return;
		 }
	 }
 }

 /*
 Takes the specified number of chips from the player, adds it to the amount they have bet, and adds it to the pot
 */
 void Game::add_to_pot(player& p, unsigned int amount){
	 p.chips -= amount;
	 p.chips_bet += amount;
	 pot += amount;
 }

 //will throw if not 
 int Game::isValidNumber(std::string str){
	 return std::stoi(str);
 }
 /*
 Takes in a message to prompt users with, a function to check their input, and an error message
 Return the users answer if they responded correctly, otherwise keeps prompting the user
 */
 std::string Game::prompt_string(const char* message,  std::function<bool(std::string)> predicate, const char* err_message = "bad input."){
	 bool valid_input = false;
	 std::string answer;
	 do{
		 std::cout << std::endl << message << std::endl;
		 std::ws(std::cin);
		 std::getline(std::cin, answer);
		 
		 
		 if (predicate(answer)){
			 return answer;
		 }
		 else{
			 std::cout << err_message << "\nTry again" << std::endl;
		 }
	 } while (!valid_input);
	 return answer;
 }

 /*
 Method that takes in a message and error message
 Creates an always valid predicate and then calls the more robust prompt_string method
 */
 std::string Game::prompt_string(const char* message, const char* err_message){
	 std::function<bool(std::string)> always_valid = [](std::string){return true;};
	 return prompt_string(message, always_valid, err_message);
 }

 /*
 Takes a message to promt the user with, a function to test their input, and an error message
 If the user enters a valid number and fulfills the predicate, returns their input
 If not continues to prompt the user
 */
 int Game::prompt_int(const char* message, std::function<bool(int)> predicate, const char* err_message = "bad input."){
	 bool valid_input = false;
	 int num;
	 do{
		 std::string num_str = prompt_string(message, err_message);
		 try{
			 num = isValidNumber(num_str);
			 if(predicate(num)){

				 return num;
			 }
			 else
				 std::cout << err_message << "\nTry again" << std::endl;
		 }
		 catch (...){
			 std::cout << err_message << "\nTry again" << std::endl;
		 }
	 } while (!valid_input);
	 return num;
 }

 /*
 For when the program does not specify a function for valid inputs
 Creates an always valid functions and then calls the other prompt_int function
 */
 int Game::prompt_int(const char* message, const char* err_message){
	 std::function<bool(int)> always_valid = [](int){return true; };
	 return prompt_int(message, always_valid, err_message);
 }

//Destructor
Game::~Game(){
		//Doesn't do anything
	}
