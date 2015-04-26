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


std::shared_ptr<Game> Game::game;

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

//Makes a shared pointer instance
 std::shared_ptr<Game> Game::instance(){
	if (!game)
		throw instance_not_available;
	std::shared_ptr<Game> g_copy(game);
	return g_copy;
}

 //Allow user to specify game type but if its not fivecarddraw
 void Game::start_game(const char* type){
	if (game)
		throw game_already_started;

	std::string game_type = type;
	if (game_type.find("FiveCardDraw")){

		throw unknown_game;

	}
	game = std::static_pointer_cast<Game>(std::make_shared<FiveCardDraw>());

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