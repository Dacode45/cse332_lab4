/*
cse332_lab3.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Contains the main function for the program that creates a game with players defined in the console
*/

#include "stdafx.h"
#include <iostream>
#include "json_parser.h"
#include "cards.h"
#include "Game.h"
#include "FiveCardDraw.h"

using namespace std;

const char* program_name;

const int expected_num_args = 4;
enum args{pname, game_name, player1, player2};

int main(int argc, char* argv[])
{
	program_name = argv[args::pname];
	
	//Check proper number of cmd line arguments
	if (argc < expected_num_args){

		handleErrMessages(NOTENOUGHARGUMENTS);
		return NOTENOUGHARGUMENTS;

	}

	//Start the game
	Game game;
	try{

		game.start_game(argv[args::game_name]);

	}

	catch(int e){ //game not found

		handleErrMessages(e);
		return e;

	}

	shared_ptr<Game> game_instance;
	try{

		game_instance = game.instance();

	}
	catch (int e){

		handleErrMessages(e);
		return e;

	}

	//Try adding the two players from main tot he game
	try{

		game_instance->add_player(argv[args::player1]);
		game_instance->add_player(argv[args::player2]);

	}
	catch (int e){

		cout << "Could not add the specified players";
		return e;

	}

	//As long as there are two or more players keep playing the game
	while (game_instance->size() >= 2){

		try{

			if (int brError = game_instance->before_round()){

				handleErrMessages(brError);

			}
			if (int rError = game_instance->round()){

				handleErrMessages(rError);

			}
			if (int arError = game_instance->after_round()){

				handleErrMessages(arError);

			}

		}

		catch(int e){

			game.stop_game();
			cout << "failure " << e;
			handleErrMessages(e);
			return e;

		}
		
	}

	
}

