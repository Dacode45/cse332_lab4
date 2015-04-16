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

Game::~Game(){
		//Doesn't do anything
	}
