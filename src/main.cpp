/*
Version 1.1
Soon - score and hints v1.2
Author thenewkgb
Title 13 Ball

=== todo ===

Images - Title
				Credits
				Instructions
				Options

Win screen - particle FX

Score - you plus a point if you
             manually clear the table in time
             but keep your score if rerack
             no --score for rerack
             gives player more chance
             
             i played for 10 minutes and
             couldnt score
             perhaps 12 is too high
             but people know what randomly
             generated is
             hence, "Button Basher"
             
Highscore - timer, date, name
             
SPARES - more feedback for spare
				if i can add a shining/melting
				transition perhaps

HARD - if i keep colour tints remember last colour
except i haven't studied pointers

HINTS - show unavailable balls
			show total
			allow ClearAll selected

=== bugs ===

Bug #1 - CurrentSpare - does it work?
				why does MAX-spares+1 work?
				MAX = 10
				vector 0-9
                
===== hey, i fixed something!

Fixed - Delay and PollEvent
			disables FINGERUP events
			before delay
			
Fixed - i updated table.balls to use
			test_rects for consistency
			each ball has its own Rect
			and i didnt update that

Fixed - last ball touches does not highlight

Fixed - i cleared the board but the score
                didnt increase/beginBoard

Fixed - if currentSpare is max 
           then the ball fails to be erased
           but if you scroll it has gone
           
Fixed - added End and X ball to spares
*/

// includes removed

#include "game.hpp"
#include "except.hpp"
#include <iostream>

// structs moved

// class Game moved

// load functions moved

// class game definitions removed




//
// main
//

int main(int argc, char *argv[])
{
	Game game{};

 // please remove if(true) throw from game.cpp loadSFX()
 // i tested some exceptions and realised i
 // did not include loading methods in try block

	// prepare for game or fail
	if (game.init() != 0)
	{
		return -1;
	}
	else if (game.loadGameMusic() != 0)
	{
		return -1;
	}
	else if (game.loadGameSFX() != 0)
	{
		return -1;
	}

	// it worked
	else
	{
		game.beginBoard();
	}

	try
	{
		while (!game.isQuit)
		{
			game.checkInput();
			game.updateUI();
		}
	}
	
	catch (SparesException)
	{
		std::cerr << "\"KErr : Main loop failed. \"";
		return 1;
	}
	catch(RunError)
	{
		std::cerr << "\"KErr : Run time error\"";
		return 1;
	}
	catch(...)
	{
		std::cerr << "\"KErr : Unknown Exception\"";
		return 2;
	}

	game.quit();
	return 0;
}