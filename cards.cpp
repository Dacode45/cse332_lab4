/*
card.cpp
Jacob Pepe-jacobpepe@wustl.edi
Tomer Sabo-tomersabo@wustl.edu
David Ayeke-ayekedavidr@wustl.edu
Implementation of functions declared in cards.cpp for parsing cards and getting their ranks, comparing them, and other program functions
*/

#include "stdafx.h"
#include "cards.h"


#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <string>


int parseCardFile(const char* filename, std::vector<Card> &cards){
	//Which Error to Return
	int toReturn = SUCCESS;
	std::string white_space_buffer; // used for skipping whitespace
	std::ifstream in(filename);
	std::string line;
	if (in.is_open()){
		in >> std::ws;
		while (std::getline(in, line)){
				
			if (in.fail()){
				in.clear();
				handleErrMessages(program_name, "Error durring File Handling. Will attempt to continue.");
				toReturn = ERRORDURINGFILEREADING;
			}
			
			
				std::istringstream iss(line);
				if (iss.rdbuf()->in_avail() == 0){
					continue;
				}
				std::vector<Card> hand;

				int cardsAdded = 0;
				char rankChar, suitChar;
				iss >> std::ws;
				while (iss >> rankChar){
					iss >> std::ws;
					iss >> suitChar;
					if (suitChar == '0'){
						rankChar = '0';
						iss >> std::ws;
						iss >> suitChar;
					}
					if (suitChar == '/' || rankChar == '/'){// reach end or comment
						break;
					}
					iss >> std::ws;

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
					
					hand.push_back(c);
					cardsAdded++;
					
				}
				
				if (cardsAdded != 5){
					
					handleErrMessages(program_name, ("Hands should only have 5 cards: "+line).c_str());

				}
				else{
					std::move(hand.begin(), hand.end(), std::back_inserter(cards));
				}
			
				in >> std::ws;
		}
		in.close();
		return toReturn;
	}
	else{
		handleErrMessages(program_name, "Couldn't open the file");
		return CANTOPENFILE;
	}
}

char* getCmdOption(char ** begin, char** end, const std::string & option){
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end){
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option){
	return std::find(begin, end, option) != end;
}



/*
How it works.
Each card gets a weight depending on the individual ranking scheme for that hand
The weight is multiplied by the rank to determin that cards value. Values are added
cards with a higher value are better hands than cards with a lesser value.
For this to work the weights must satisfy the inequality
a> 13*b + 13*c + 13*d + 13*e; 
b> 13*c + 13*d + 13*e; 
c > 13*d + 13*e;
d>13*e
setting e to 1
d = 14, c = 196, b = 2744, a = 38416 satisfies this inequality.
The max rank for a hand is therefore a+b+c+d+e or 537823.
I can order the hands by multiplying a rank by some integer below
This allows me to sort and check if hands are equal.
*/


unsigned int checkHand(const std::vector<Card> &cards){
	//std::sort(cards.begin(), cards.end());
	int numSpades = 0, numClubs = 0, numDiamonds = 0, numHearts = 0, numSame = 0, numConsecutive = 0;
	int numBreaks = 0;//Check for when cards stop being same value

	
	for (auto it = cards.cbegin(); it != cards.cend()-1; ++it){
		switch (it->suit){
		case Card::SUIT::clubs:
			numClubs++;
		break;
		case Card::SUIT::spades:
			numSpades++;
		break;
		case Card::SUIT::diamonds:
			numDiamonds++;
		break;
		case Card::SUIT::hearts:
			numHearts++;
		break;
		}
		if (it->rank == (it + 1)->rank){
			numSame++;
		}
		else{
			numBreaks++;
			if (it->rank == ((it + 1)->rank - 1)){
				numConsecutive++;
			}
		}
	}

	//check last card suit
	switch ((cards.end()-1)->suit){
	case Card::SUIT::clubs:
		numClubs++;
		break;
	case Card::SUIT::spades:
		numSpades++;
		break;
	case Card::SUIT::diamonds:
		numDiamonds++;
		break;
	case Card::SUIT::hearts:
		numHearts++;
		break;
	}

	//check strait
	//Understanding the math.
	//Return KINDOFHAND*(5*rank of highest + 4*rane of next highest ...1*lowest)
	//cards are sorted from lowest to highest. Highest card is index 4;
	if (numConsecutive == 4){
		//check strait flush
		if (numSpades == 5 || numClubs == 5 || numDiamonds == 5 || numHearts == 5){
			//two straight flush hands are sorted by the highest card in each hand;
			return STRAITFLUSH *CARDWEIGHT_TOTAL + (CARDWEIGHT_A + CARDWEIGHT_B + CARDWEIGHT_C + CARDWEIGHT_D + CARDWEIGHT_E)*cards[4].rank;
		}
		//two straight hands are sorted by the highest card in each hand;
		return STRAIT * CARDWEIGHT_TOTAL + (CARDWEIGHT_A + CARDWEIGHT_B + CARDWEIGHT_C + CARDWEIGHT_D + CARDWEIGHT_E)*cards[4].rank;

	}
	//suits.
	if (numSpades == 5 || numClubs == 5 || numDiamonds == 5 || numHearts == 5){
		//two flush hands are sorted by highest ranking card, then by the next highest ranking card, etc.;
		return FLUSH *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[4].rank + CARDWEIGHT_B * cards[3].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D * cards[1].rank + CARDWEIGHT_E * cards[0].rank);
		
	}
	
	if (numSame == 3){//could be four of a kind or full house
		//in four of a kind the third card always equals the second and the fourth
		if ((cards.begin() + 2)->rank == (cards.begin() + 1)->rank && (cards.begin() + 2)->rank == (cards.begin() + 3)->rank){
			//two four of a kind hands are sorted by the rank of the 4 cards that have the same rank;
			//due to sorting cards 1,2,and 3 (zero indexed) are always in THE FOUR KIND
			//I picked 2 because computer sciencentist love 2
			return FOURKIND*CARDWEIGHT_TOTAL + (CARDWEIGHT_A + CARDWEIGHT_B + CARDWEIGHT_C + CARDWEIGHT_D + CARDWEIGHT_E)*cards[2].rank;
		
		}
		else{
			//two full house hands are sorted by the rank of the 3 cards that have the same rank and then by the rank of the 2 cards that have the same rank;
			//full house is 3 of a kind followed by two of a kind, i need to adjust for if the three of a kind is at the begining or the end.
			//simple enough if card at position 1 == card at position 2 it is at the BEGINNING

			if (cards[1].rank == cards[2].rank){
				return FULLHOUSE *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[0].rank + CARDWEIGHT_B * cards[1].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D * cards[3].rank + CARDWEIGHT_E * cards[4].rank);
			}else{

				return FULLHOUSE *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[2].rank + CARDWEIGHT_B * cards[3].rank + CARDWEIGHT_C * cards[4].rank + CARDWEIGHT_D * cards[0].rank + CARDWEIGHT_E * cards[1].rank);
			}
		
		}	
	}
	if (numSame == 2){ // three of kind or 2 pair
		//check if ends are equal for middle card. only three of a kind has this property
		//two three of a kind hands are sorted by the rank of the 3 cards that have the same rank;

		if (cards.begin()->rank == (cards.begin() + 2)->rank){
			return THREEKIND *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[0].rank + CARDWEIGHT_B * cards[1].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D + CARDWEIGHT_E);

		}if( (cards.begin() + 4)->rank == (cards.begin() + 2)->rank){
			return THREEKIND *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[4].rank + CARDWEIGHT_B * cards[3].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D + CARDWEIGHT_E);

		
		}
		if ((cards.begin() + 1)->rank == (cards.begin() + 2)->rank && (cards.begin() + 2) == (cards.begin() + 3)){ //check if middle cards are equal
			return THREEKIND *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[1].rank + CARDWEIGHT_B * cards[2].rank + CARDWEIGHT_C * cards[3].rank + CARDWEIGHT_D + CARDWEIGHT_E);
			
		}
		//two two pair hands are sorted by the rank of the higher pair, then by the rank of the lower pair, and then by the 5th card;
		
		//Determing the sorting order of two pair is more tedius than complicated.
		//The card that is not a pair is either in the middle, the beginning, or the end
		//Once we know that cards location we know the locations of the two pairs.
		//we sort those cards by the above.
		
		//middle
		if ((cards.begin() + 2)->rank != (cards.begin() + 3)->rank && (cards.begin() + 2)->rank != (cards.begin() + 1)->rank){
			//paris are at positions 0,1 & 3,4
			//check if higher rank is at position 1 or 3.
			if ((cards.begin() + 1)->rank > (cards.begin() + 3)->rank){
				return TWOPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[0].rank + CARDWEIGHT_B * cards[1].rank + CARDWEIGHT_C * cards[3].rank + CARDWEIGHT_D * cards[4].rank + CARDWEIGHT_E * cards[2].rank);

			}
			else{//equalto is trivial
				return TWOPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[3].rank + CARDWEIGHT_B * cards[4].rank + CARDWEIGHT_C * cards[0].rank + CARDWEIGHT_D * cards[1].rank + CARDWEIGHT_E * cards[2].rank);

			}
		}
		//beginning'
		else if ((cards.begin())->rank != (cards.begin() + 1)->rank){
			//paris are at positions 1,2 & 3,4
			//check if higher rank is at position 2 or 3.
			if ((cards.begin() + 2)->rank > (cards.begin() + 3)->rank){
				return TWOPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[2].rank + CARDWEIGHT_B * cards[1].rank + CARDWEIGHT_C * cards[3].rank + CARDWEIGHT_D * cards[4].rank + CARDWEIGHT_E * cards[0].rank);

			}
			else{//equalto is trivial
				return TWOPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[3].rank + CARDWEIGHT_B * cards[4].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D * cards[1].rank + CARDWEIGHT_E * cards[0].rank);

			}
		}
		//end
		else{
			//pairs are at positions 0,1 & 2,3
			//check if higher rank is at position 1 or 3.
			if ((cards.begin() + 1)->rank > (cards.begin() + 3)->rank){
				return TWOPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[0].rank + CARDWEIGHT_B * cards[1].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D * cards[3].rank + CARDWEIGHT_E * cards[4].rank);

			}
			else{//equalto is trivial
				return TWOPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[2].rank + CARDWEIGHT_B * cards[3].rank + CARDWEIGHT_C * cards[0].rank + CARDWEIGHT_D * cards[1].rank + CARDWEIGHT_E * cards[4].rank);

			}
		}
	}
	if (numSame == 1){
		//two one pair hands are sorted by the rank of the pair, then by the highest unpaired card, then the next highest unpaired card, and then by the next highest unpaired card;
		//one card of the pair will always be at positions 1,2,3. I then check adjacents
		if ((cards.begin() + 1)->rank == (cards.begin() + 0)->rank){
			return ONEPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[0].rank + CARDWEIGHT_B * cards[1].rank + CARDWEIGHT_C *cards[4].rank + CARDWEIGHT_D * cards[3].rank + CARDWEIGHT_E* cards[2].rank);

		}
		if ((cards.begin() + 1)->rank == (cards.begin() + 2)->rank){
			return ONEPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[1].rank + CARDWEIGHT_B * cards[2].rank + CARDWEIGHT_C *cards[4].rank + CARDWEIGHT_D* cards[3].rank + CARDWEIGHT_E* cards[0].rank);

		}
		if ((cards.begin() + 2)->rank == (cards.begin() + 3)->rank){
			return ONEPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A * cards[2].rank + CARDWEIGHT_B * cards[3].rank + CARDWEIGHT_C*cards[4].rank + CARDWEIGHT_D*cards[1].rank + CARDWEIGHT_E*cards[0].rank);

		}
		if ((cards.begin() + 3)->rank == (cards.begin() + 4)->rank){
			return ONEPAIR *CARDWEIGHT_TOTAL + (CARDWEIGHT_A* cards[3].rank + CARDWEIGHT_B * cards[4].rank + CARDWEIGHT_C * cards[2].rank + CARDWEIGHT_D * cards[1].rank + CARDWEIGHT_E* cards[0].rank);

		}
	}
	//two unranked hands are sorted by highest ranking card, then by the next highest ranking card, etc.
	
	return UNRANKED *CARDWEIGHT_TOTAL + (CARDWEIGHT_A* cards[4].rank + CARDWEIGHT_B* cards[3].rank + CARDWEIGHT_C* cards[2].rank + CARDWEIGHT_D* cards[1].rank + CARDWEIGHT_E*cards[0].rank);
}

bool handComparator(std::vector<Card>& hand1, std::vector<Card>& hand2){
	float hand1_value = std::ceil(((float)checkHand(hand1)) / ((float)(CARDWEIGHT_TOTAL)));
	float hand2_value = std::ceil(((float)checkHand(hand2)) / ((float)(CARDWEIGHT_TOTAL)));
	return hand1_value < hand2_value;
}

bool poker_rank(const Hand& h1, const Hand& h2){
	 
	 unsigned int h1_value = checkHand(h1.getCards());
	 unsigned int h2_value = checkHand(h2.getCards());
	 return h1_value > h2_value;
}

char* hand_type(const Hand& h){
	int hand_value = static_cast<int>(std::floor(((float)checkHand(h.getCards())) / ((float)(CARDWEIGHT_TOTAL))));
	
	switch (hand_value)
	{
	case UNRANKED:
		return "UNRANKED";
	case ONEPAIR:
		return "ONEPAIR";
	case TWOPAIR:
		return "TWOPAIR";
	case THREEKIND:
		return "THREEKIND";
	case STRAIT:
		return "STRAIT";
	case FLUSH:
		return "FLUSH";
	case FULLHOUSE:
		return "FULLHOUSE";
	case FOURKIND:
		return "FOURKIND";
	case STRAITFLUSH:
		return "STRAITFLUSH";
	
	default:
		return "UNRANKED";
	}
}

int printCards( std::vector<Card> &cards){
	//EX Three of Diamonds
	try{

		
		//sort hands
		std::vector<std::vector<Card>> hands;
		for (auto it = cards.begin(); it != cards.end(); it += 5){
			
			std::vector<Card> hand;
			std::copy(it, it + 5, std::back_inserter(hand));
			hands.push_back(hand);
			
		}
		std::sort(hands.begin(), hands.end(), handComparator);
		//print hands
		for (auto it = hands.begin(); it != hands.end(); it++){
			
			auto h = *it;
			

			unsigned int hand_value = (unsigned int)std::ceil(((float)checkHand(h)) / ((float)(CARDWEIGHT_TOTAL)));

			switch (hand_value)
			{//must use + 1 to convert zero indexing to 1 indexing
				case UNRANKED + 1:
					std::cout << "Unranked" << std::endl;
				break;

				case ONEPAIR + 1:
					std::cout << "One Pair" << std::endl;
					break;

				case TWOPAIR + 1:
					std::cout << "Two Pair" << std::endl;
					break;

				case THREEKIND + 1:
					std::cout << "Three of a Kind" << std::endl;
					break;

				case STRAIT + 1:
					std::cout << "Strait" << std::endl;
					break;

				case FLUSH + 1:
					std::cout << "Flush" << std::endl;
					break;

				case FULLHOUSE + 1:
					std::cout << "Full House" << std::endl;
					break;
				case FOURKIND + 1:
					std::cout << "Four of a Kind" << std::endl;
					break;
				case STRAITFLUSH + 1:
					std::cout << "Strait Flush" << std::endl;
					break;
			}

		}
		//Print all Cards

		std::sort(cards.begin(), cards.end());

		for (auto it = cards.begin(); it != cards.end(); ++it){
			std::cout << Card::rankMap[it->rank] << Card::suitMap[it->suit] << std::endl;
		}
		return SUCCESS;
	}
	catch (int e){
		handleErrMessages(program_name, "C++ Error " + e);
		return FAILEDTOPRINTCARDS;
	}
	
}

int usageMessage(const char* pName){
	return usageMessage(pName, "");
	
}

int usageMessage(const char* pName, const char* errMsg){
	try{

		std::cout << "Usage: " << pName << " game_name[\"FiveCardDraw\"] player1_name player2_name" << std::endl; 
		std::cout << errMsg;
		return PRINTEDUSAGEMESSAGE;
	}
	catch (int e){
		std::cout << "C++ Error " << e;
		return FAILEDTOPRINTUSAGE;
	}
}

void handleErrMessages(int err){
	handleErrMessages(program_name, err);
}

void handleErrMessages(const char* pName, int err){

	switch (err){
	case SUCCESS:// SUCCESS:

		break;
	case CANTOPENFILE://CANTOPENFILE:
		std::cout << "\nCouldn't Open File. May not Exist\n";
		usageMessage(pName);
		break;
	case IMPROPERFILEFORMAT://IMPROPERFILEFORMAT:
		std::cout << "\nCard Format incorrect. Check Readme on the format\n";
		break;
	case ERRORDURINGFILEREADING:// ERRORDURINGFILEREADING:
		std::cout << "\nFile opened, but there was an error while reading. File may be currupt\n";
		break;
	case FAILEDTOPRINTUSAGE://FAILEDTOPRINTUSAGE:
		std::cout << "\nThis should never ever happen. Failed to print usage message\n";
		break;
	case PRINTEDUSAGEMESSAGE://PRINTEDUSAGEMESSAGE:
		std::cout << "\nPrinted Usage Message \n";
		break;

	case FAILEDTOPRINTCARDS: //FAILEDTOPRINTCARDS
		std::cout << "\nThis should not happen. C++ error: Unable to access vector<Cards>.\n";
		break;
	case BADNUMBEROFCARDSINHAND: //BADNUMBEROFCARDSINHAND
		std::cout << "\nThere should only ever be 5 cards in your hand at a time. no more no less\n";
		break;
	case WRONGCOMMANDLINEARGS:
		std::cout << "\nYou are using incorrect command line arguments \n";
		usageMessage(program_name);
		break;
	case NOTENOUGHARGUMENTS:
		std::cout << "\nNeed Command Line arguments\n";
		usageMessage(program_name);
		break;
	case NOTENOUGHARGUMENTS_SHUFFLE:
		std::cout << "\nNeed filename too\n";
		usageMessage(program_name);
		break;
	case NOCOMMANDLINEARGS:
		std::cout << "\nYou need commands to run this program\n";
		usageMessage(program_name);
		break;

	case instance_not_available:
		std::cout << "\nCould not launch game. Try again\n";
		usageMessage(program_name);
		break;

	case unknown_game:
		std::cout << "\n Unknown game type. Try again\n";
		usageMessage(program_name);
		break;

	case game_already_started:
		std::cout << "\n Game's already being played. Can not create a new one\n";
		usageMessage(program_name);
		break;
	
	case already_playing:
		std::cout << "\nPlayer already playing.\n";
		break;

	default:
		break;
	}
}

void handleErrMessages(const char* pName, const char* errMsg){
	std::printf("%s: %s\n", pName, errMsg);
}