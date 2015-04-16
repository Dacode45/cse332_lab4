/*
cards.h
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Definitions of const variables to be used throughout the program and methods for usage and handling errors
*/

#ifndef CARDS_H
#define CARDS_H

#include<vector>

#include "Hand.h"
#include "card.h"
#include "Deck.h"

class Hand;
class Deck;

//char for program name, extern because defined elsewhere
extern const char* program_name;


//Parses ; file, and puts contents into a vector. Returns Success or Error.
int parseCardFile(const char* filename, std::vector<Card> &cards);
//Print out cards in vector
int printCards( std::vector<Card> &cards);
//Usage Message 
int usageMessage(const char* programName);
int usageMessage(const char* pName, const char* errMsg);
//couts error message
void handleErrMessages(int err);
void handleErrMessages(const char* pName, int err);
void handleErrMessages(const char* pName, const char* errMsg);

//command line args
char* getCmdOption(char ** begin, char** end, const std::string & option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);

//lab 2
bool poker_rank(const Hand& h1, const Hand& h2);
char* hand_type(const Hand& h);
unsigned int checkHand(const std::vector<Card> &cards);

//Defines codes for types of hands
const unsigned int UNRANKED = 0,
ONEPAIR = 1,
TWOPAIR = 2,
THREEKIND = 3,
STRAIT = 4,
FLUSH = 5,
FULLHOUSE = 6,
FOURKIND = 7,
STRAITFLUSH = 8;

//Defines weights for highest card value, second highest card value, all the way to fifth card value. (first card has higher weights than all others, etc)
const unsigned int CARDWEIGHT_A = 38416,
CARDWEIGHT_B = 2744,
CARDWEIGHT_C = 196,
CARDWEIGHT_D = 14,
CARDWEIGHT_E = 1;

//Highest total card weight
const unsigned int CARDWEIGHT_TOTAL = (CARDWEIGHT_A + CARDWEIGHT_B + CARDWEIGHT_C + CARDWEIGHT_D + CARDWEIGHT_E) * 13;

//Define all error codes
const int SUCCESS= 0;
const int CANTOPENFILE= 1;
const int IMPROPERFILEFORMAT= 2;
const int ERRORDURINGFILEREADING =3;
const int FAILEDTOPRINTUSAGE = 4;
const int PRINTEDUSAGEMESSAGE= 5;
const int FAILEDTOPRINTCARDS= 6;
const int BADNUMBEROFCARDSINHAND =7;
const int TOOMANYARGUMENTS = 8;
const int NOTENOUGHARGUMENTS = 9;
const int WRONGCOMMANDLINEARGS = 10;
const int NOTENOUGHARGUMENTS_SHUFFLE = 11;
const int NOCOMMANDLINEARGS = 12;
const int HANDOUTOFBOUNDS = 13;
const int INVALIDJSONOBJ = 14;
const int JSONKEYNOTFOUND = 15;
const int instance_not_available = 16;
const int game_already_started = 17;
const int unknown_game = 18;
const int no_game_in_progress = 19;
const int already_playing = 20;
const int NOTENOUGHCARDSTODEAL = 21;
const int BADINPUT = 22;

#endif // !CARDS_H
