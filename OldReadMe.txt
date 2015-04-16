Compiler deconstructor okay because 
I did not allocate new memmory from the heap. No need
for a custom destructor

Poker rank is defined in cards.h

Extra Credit
1) Exceptions
There is only one exception and that is with the constructor
of a deck that takes a file name. It throws an int if the 
file does not exist to load cards from. This int is consistent
with the error codes defiend throughout the program. 
However, I do not bother with this constructor as the load
function is public and I can load a file. If it fails I 
can check that with an if statement.

2)
A.12-A.15)
I don't use pointers or c styled arrays in the logic of my code
outside of argv. I do have a function that deals with a 
char** but it is unused, and writen just in case. I use 
vectors and strings for my app logic so I don't have to worry 
too much about deferencing pointers. when using iterators,
I go through the beginning to the end by going to the next
iterator one at a time until reaching the end. This is fine 
because it is imposible to count from 1 to 10 and skip 7.

3)
A.16)
I wrote this program in one sitting. Compiled and ran perfectly
on the first try. (Like a boss).

4)
B.12) I have  two constructors for Deck. A safe constructor
that never fails. and an unsafe constructor taking a filename
that can fail and will throw an exception in that case.
A.13) In order to maintain readability, I first assume
that the first argument passed to the program is the filename
I then check if this is actually -shuffle. if it is i then 
use the second argument passed. This is better than
if(file_name = argv[lab2_cmd_args::file_name] == "-shuffle"){
	file_name = argv[lab2_cmd_args::shuffle];
}

5)
B.18)Decks shouldn't be copied.
it is also private
So I used Deck(const Deck& d) = delete;
To be honest, Deck's aren't singletons so i don't think
I should have prevented them from being copied. 


6)
B.31)
I always use protection.

No Errors