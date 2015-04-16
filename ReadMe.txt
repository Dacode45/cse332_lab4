Lab 3
David Ayeke, Jacob Pepe, Tomer Sabo

Our program allows two or more players to play poker against each other. Players are dealt cards and then can decide 
which if any cards they want to discard. Then all the players hands are compared and a winner is declared. After a round wins
and losses are stored for each player and players can decide whether or not they want to leave.

A7 - Error message codes can be found in cards.h
A11 - Extra whitespace is permitted in card input files

Files are explained in a comment header at the top, main function is located in cse332_lab3.cpp

Errors) We didn't run into any errors while testing (hooray).

Testing)
Command lines that all resulted in this output-"Need Command Line arguments Usage: cse332_lab3.exe game_name["FiveCardDraw"] player1_name player2_name"
-"cse332_lab3.exe"
-"cse332_lab3.exe FiveCardDraw"
-"cse332_lab3.exe FiveCarddraw a"

-"cse332_lab3.exe FiveCarddraw a b" results in " Unknown game type. Try again"
Program is designed to ignore arguments past when they have already specified two players


Extra credit) We implemented a boolean isrobot in the player class to keep track of auto players. Also there is an enum win_low
to track if the player won, lost, or came in last. Then we added a getdecision method to decide which cards an auto player
should discard and a loop in after_round that decides whether or not a player leaves.

Example 1)
"Player: b won
b games won: 3 games_lost 1 : 2C 2D 6D 10H 10S  TWOPAIR
d games won: 0 games_lost 2 : 5H 8D QD KD KH    ONEPAIR
c games won: 0 games_lost 2 : 8S JC QS KC AH    UNRANKED

 current players are b, d,"
c left as the loser
Example 2)
"Player: e won
e games won: 1 games_lost 0 : 2C 2S 5D JD JH    TWOPAIR
f games won: 0 games_lost 1 : 3C 7C KC KD AC    ONEPAIR
b games won: 3 games_lost 2 : 8C 10C JC JS AH   ONEPAIR
d games won: 0 games_lost 3 : 3D 3S 4C 8H 9D    ONEPAIR

 current players are b, d,      e,"
f left as the loser but not last place

Example 3)
"b : 4S 6C 7C JD KD
d : 2D 5H 9C KC AH
r : 3D 4H 6S 7H QS
y : 5S 7D 10C 10H KS

b : 5D JD QD QH KD

d : 3H 8H QC KC AH

r : 2H 3S 6H 7H QS

y : 5S 6D 10H JC JS"
Shows auto players changing their hands based on the getdecision method
