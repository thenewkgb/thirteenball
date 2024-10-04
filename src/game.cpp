#include "game.hpp"
#include "load.hpp"
#include "except.hpp"

#include <iostream>

constexpr int ROWONESIZE = 3;
constexpr int ROWTWOSIZE = 4;
constexpr int ROWTHREESIZE = 5;
constexpr int ROWFOURSIZE = 6;

int Game::loadGameMusic()
{
	std::string m{};
	m = "sfx/song18.mp3";

	music1 = loadMusic(m.c_str());
	if (music1 == NULL)
		return -1;
	/*
    more music here
    */

	return 0;
}

int Game::loadGameSFX()
{
	if(true)
	{
		quit();
		throw RunError{};
	}
	
	std::string s{};
	//s = "sfx/click3.wav";
	s = "sfx/bleep24Stereo.wav";
	click = loadSound(s.c_str());

	std::string s2{};
	s2 = "sfx/exitLow.wav";
	win = loadSound(s2.c_str());

	if (click == NULL || win == NULL)
		return -1;
}

void Game::loadGFX()
{
	ballSize = (unsigned int)sw / 7;

	quitButton.x = sw-ballSize*3;
	quitButton.y = 150;
	quitButton.w = ballSize*2;
	quitButton.h = ballSize;

	reRackButton.x = ballSize;
	reRackButton.y = 150;
	reRackButton.w = ballSize*2;
	reRackButton.h = ballSize;

	timer.x = ballSize;
	timer.y = ballSize*3;
	timer.w = sw-ballSize*2;
	timer.h = ballSize/2;

	timeRemaining.x = ballSize;
	timeRemaining.y = ballSize*3;
	timeRemaining.w = 1; // computed
	timeRemaining.h = ballSize/2;

	scoreOnePos.x = (sw / 2) - 64;
	scoreOnePos.y = 0;
	scoreOnePos.w = 128;
	scoreOnePos.h = 128;

	spareButton.x = 350;
	spareButton.y = (sh / 2) + 300;
	//kev
	spareButton.x = sw / 2;
	spareButton.y = sh - ballSize * 5;
	spareButton.w = ballSize;
	spareButton.h = ballSize;
	//spareButton.w = 150;
	//spareButton.h = 150;

	sparesPos.x = 200;
	sparesPos.y = (sh / 2) + 300;
	//kev
	sparesPos.x = sw / 2 - ballSize * 2;
	sparesPos.y = sh - ballSize * 5;
	sparesPos.w = ballSize;
	sparesPos.h = ballSize;
	//sparesPos.w = BALLSIZE;
	//sparesPos.h = BALLSIZE;

	background.x = 0;
	background.y = 0;
	background.w = sw;
	background.h = sh;

	// kev
	// score debug
	test_total_rect.w = 60;
	test_total_rect.h = 60;
	test_total_rect.x = sw / 2;
	test_total_rect.y = sh - 60;
}

int Game::init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		return -1;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096) == -1)
	{
		return -1;
	}
	
	// kev
	if (TTF_Init() != 0)
	{
		return -1;
	}

	// kev
	test_font =
		TTF_OpenFont("/system/fonts/Roboto-Regular.ttf", 60);
	if (test_font == NULL)
	{
		return -1;
	}

	window = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	if (window == NULL)
	{
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_GetRendererOutputSize(renderer, &sw, &sh);

	srand(time(0));
	//startTime = time(0);

	return 0;
}

void Game::quit()
{
	clearValues();

	Mix_FreeChunk(click);
	Mix_FreeMusic(music1);
	Mix_FreeMusic(music2);
	Mix_FreeMusic(music3);
	click = NULL;
	music1 = NULL;
	music2 = NULL;
	music3 = NULL;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = NULL;
	window = NULL;

	Mix_CloseAudio();
	Mix_Quit();
	// kev
	TTF_CloseFont(test_font);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::chooseSong()
{
	if (Mix_PlayingMusic() == 0)
	{
		switch (songNo)
		{
		case 0:
		{
			Mix_PlayMusic(music1, 0);
			++songNo;
			break;
		}
		case 1:
		{
			Mix_PlayMusic(music1, 0);
			songNo = 0;
			break;
		}
		}
	}
}

void Game::updateUI()
{
	checkTime();
	// its a bit overkill to refresh the screen
	// every frame whereas before each
	// touch
	updateScreen();
}

void Game::beginBoard()
{
	Mix_PlayChannel(-1, win, 0);

	clearValues();
	loadGFX();
	setupTable();
	updateScreen();

	startTime = time(0);

	Mix_PlayMusic(music1, 0);
}

void Game::setupTable()
{
	// bkgrd
	std::string s = "gfx/thirteen_backimg3.png";
	table.backgroundimg = loadImage(renderer, s.c_str());
	// reRack
	std::string r = "gfx/thirteenRack.png";
	table.reRackimg = loadImage(renderer, r.c_str());
	// quit
	std::string q = "gfx/thirteenQuit.png";
	table.quitimg = loadImage(renderer, q.c_str());
	// loop spares
	std::string t = "gfx/turnball2.png";
	table.turnballimg = loadImage(renderer, t.c_str());

	//
	// positions
	//
	
	for (int i = 2; i > -1; --i)
	{
		// kev
		//int x = (sw / 2) + (120 * i) - 180;
		int x = (sw / 2) + (ballSize * i) - ballSize * 1.5;
		Ball temp;
		temp.w = ballSize;
		temp.h = ballSize;
		temp.x = x;
		//temp.y = (sh / 2) - 180;
		temp.y = sh / 2 - ballSize * 3;
		table.balls.push_back(temp);
	}

	for (int i = 2; i > -2; --i)
	{
		// kev
		//int x = (sw / 2) + (120 * i) - 120;
		int x = sw / 2 + ballSize * i - ballSize;
		Ball temp;
		temp.w = ballSize;
		temp.h = ballSize;
		temp.x = x;
		//temp.y = (sh / 2) - 120;
		temp.y = sh / 2 - ballSize * 2.5;
		table.balls.push_back(temp);
	}

	for (int i = 2; i > -3; --i)
	{
		// kev
		//int x = (sw / 2) + (120 * i) - 60;
		int x = sw / 2 + ballSize * i - ballSize * 0.5;
		Ball temp;
		temp.w = ballSize;
		temp.h = ballSize;
		temp.x = x;
		temp.y = sh / 2 - ballSize * 2;
		//temp.y = (sh / 2) - 60;
		table.balls.push_back(temp);
	}

	for (int i = 2; i > -4; --i)
	{
		// kev
		//int x = (sw / 2) + (120 * i);
		int x = sw / 2 + ballSize * i;
		Ball temp;
		temp.w = ballSize;
		temp.h = ballSize;
		temp.x = x;
		temp.y = sh / 2 - ballSize * 1.5;
		//temp.y = (sh / 2);
		table.balls.push_back(temp);
	}

	// randomly place numbers 1 to 13
	// into table
	// as we have their positions sorted above
	//
	// we need separate textures
	// for each ball

	// kev
	// give Balls Rects
	for (int i = 0; i < table.balls.size(); ++i)
	{
		test_rects[i].x = table.balls[i].x;
		test_rects[i].y = table.balls[i].y;
		test_rects[i].w = table.balls[i].w;
		test_rects[i].h = table.balls[i].h;
	}

	for (int i = 0; i < table.balls.size(); ++i)
	{
		// kev
		// game is too hard with 12 max
		int magic_number = 11;
		int randvalue = 1 + (rand() % magic_number);

		SDL_Texture *tempTex{};
		std::string s{};
		s += "gfx/ballB"; // prefix
		s += std::to_string(randvalue) + ".png";

		tempTex = loadImage(renderer, s.c_str());
		table.balls[i].value = randvalue;
		table.balls[i].texture = tempTex;
	}

	// create spare balls
	for (int i = 0; i < SPARESMAXNUM; ++i)
	{
		int magic_number = 12;
		int randvalue = 1 + (rand() % magic_number);
		SDL_Texture *tempTex{};
		std::string s{};
		s += "gfx/ballB";
		s += std::to_string(randvalue) + ".png";
		tempTex = loadImage(renderer, s.c_str());

		Ball tempBall{};
		tempBall.texture = tempTex;
		tempBall.value = randvalue;
		table.spares.push_back(tempBall);
	}

	// spares were separated from table
	// when game grew, so add here
	
	SDL_Texture* ballEnd_text =
		loadImage(renderer, "gfx/ballEnd.png");
	SDL_Texture* ballX_tex =
		loadImage(renderer, "gfx/ballX.png");
		
	Ball tempBall{};
	tempBall.texture = ballEnd_text;
	tempBall.value = 0;
	table.spares.push_back(tempBall);
	
	tempBall.texture = ballX_tex;
	tempBall.value = 0;
	table.spares.push_back(tempBall);
	
	// test
	/*
	if(SPARESMAXNUM+2 != table.spares.size())
	{
		std::cout << " MAX " << SPARESMAXNUM
			<< " spares.size " << table.spares.size()
			<< " and + " << table.spares.size()+2;
	}
	*/

	// add some more variation
	/*
        colourSpares();
        colourBalls();
    */

	//
	// load score digits 1...10
	//
	for (int i = 0; i < 10; ++i)
	{
		MyFont tempT{};
		std::string s{};
		s += "gfx/no";
		s += std::to_string(i);
		s += "font.png";
		tempT.fontTexture = loadImage(renderer, s.c_str());
		table.fonts.push_back(tempT);
	}

	// debug
	std::cout << "Table ball size " << table.balls.size();
	std::cout << " Spares size " << table.spares.size();
}

void Game::clearValues()
{
	if (table.backgroundimg != NULL)
	{
		SDL_DestroyTexture(table.backgroundimg);
		table.backgroundimg = NULL;
	}

	if (table.reRackimg != NULL)
	{
		SDL_DestroyTexture(table.reRackimg);
		table.reRackimg = NULL;
	}

	if (table.quitimg != NULL)
	{
		SDL_DestroyTexture(table.quitimg);
		table.quitimg = NULL;
	}

	if (table.turnballimg != NULL)
	{
		SDL_DestroyTexture(table.turnballimg);
		table.turnballimg = NULL;
	}

	for (int i = 0; i < table.balls.size(); ++i)
	{
		if (table.balls[i].texture != NULL)
		{
			SDL_DestroyTexture(table.balls[i].texture);
			table.balls[i].texture = NULL;
		}
	}

	for (int i = 0; i < table.spares.size(); ++i)
	{
		if (table.spares[i].texture != NULL)
		{
			SDL_DestroyTexture(table.spares[i].texture);
			table.spares[i].texture = NULL;
		}
	}

	for (int i = 0; i < table.fonts.size(); ++i)
	{
		if (table.fonts[i].fontTexture != NULL)
		{
			SDL_DestroyTexture(table.fonts[i].fontTexture);
			table.fonts[i].fontTexture = NULL;
		}
	}

	// kev
	SDL_DestroyTexture(test_tex);
	test_tex = NULL;

	table.balls.clear();
	table.spares.clear();
	table.fonts.clear();
	totalSum.clear();
	locations.clear();

	// reset figures
	currentSpare = 0;
	sparesLeft = SPARESMAXNUM;
}

void Game::createEmpty(int i)
{
	// kev
	//test_rects[i].y += ballSize*2;
	test_rects[i].w = 0;
	test_rects[i].h = 0;
	// i changed the input loop to check
	// test_rects
	table.balls[i].w = 0;
	table.balls[i].h = 0;
	table.balls[i].show = false;
	SDL_DestroyTexture(table.balls[i].texture);
	table.balls[i].texture = nullptr;
}

void Game::colourBalls()
{
	for (int i = 0; i < table.balls.size(); ++i)
	{
		// set
		int r = 100 + (rand() % 150);
		int g = 100 + (rand() % 150);
		int b = 100 + (rand() % 150);

		SDL_SetTextureColorMod(table.balls[i].texture, r, g, b);

		table.balls[i].r = r;
		table.balls[i].g = g;
		table.balls[i].b = b;
	}
}

void Game::colourSpares()
{
	for (int i = 0; i < SPARESMAXNUM; ++i)
	{
		int r = 100 + (rand() % 150);
		int g = 100 + (rand() % 150);
		int b = 100 + (rand() % 150);

		SDL_SetTextureColorMod(table.spares[i].texture, r, g, b);

		table.spares[i].r = r;
		table.spares[i].g = g;
		table.spares[i].b = b;
	}
}

void Game::highlight(int i)
{
	SDL_SetTextureColorMod(table.balls[i].texture, 200, 200, 255);
	table.balls[i].highlighted = true;

	Mix_PlayChannel(-1, click, 0);
}

void Game::deselect(int i)
{
	SDL_SetTextureColorMod(table.balls[i].texture, 255, 255, 255);
	table.balls[i].highlighted = false;
}

void Game::handleHighlight(int i)
{
	// not separate ifs
	if (!table.balls[i].highlighted)
	{
		highlight(i);
	}
	else
	{
		deselect(i);
	}
}

void Game::hightlightSpare(int spareNum)
{
	SDL_SetTextureColorMod(table.spares[spareNum].texture, 200, 255, 200);
	table.spares[spareNum].highlighted = true;
}

void Game::deselectSpare(int spareNum)
{
	SDL_SetTextureColorMod(table.spares[spareNum].texture, 255, 255, 255);
	table.spares[spareNum].highlighted = false;
}

void Game::handleSpareHighlight(int spareNum)
{
	if (!table.spares[spareNum].highlighted)
	{
		hightlightSpare(spareNum);
	}
	else
	{
		deselectSpare(spareNum);
	}
}

bool Game::checkTableEmpty()
{
	for (int i = 0; i < table.balls.size(); ++i)
	{
		if (table.balls[i].show == true)
		{
			// ++score;
			//  return true;
			return false;
		}
	}
	// are we in time?
	if(bar>0)
		++score;
		
	return true;
}

bool Game::canClick(int i)
{
	// i >= 0 && i < 3
	if (i >= 0 && i < ROWONESIZE)
	{
		if (table.balls[i + ROWONESIZE].show == false &&
			table.balls[i + ROWONESIZE + 1].show == false)
		{
			return true;
		}
	}

	// i >= 3 && i < 7
	else if (i >= ROWONESIZE && i < ROWTWOSIZE + ROWONESIZE)
	{
		if (table.balls[i + ROWTWOSIZE].show == false && table.balls[i + ROWTWOSIZE + 1].show == false)
		{
			return true;
		}
	}

	// i >= 7 && i < 12
	else if (i >= ROWONESIZE + ROWTWOSIZE && i < ROWTHREESIZE + ROWTWOSIZE + ROWONESIZE)
	{
		if (table.balls[i + ROWTHREESIZE].show == false && table.balls[i + ROWTHREESIZE + 1].show == false)
		{
			return true;
		}
	}

	// i >= 12 && i < 18
	else if (i >= ROWTHREESIZE + ROWTWOSIZE + ROWONESIZE && i < ROWFOURSIZE + ROWTHREESIZE + ROWTWOSIZE + ROWONESIZE)
	{
		// do not need check for front balls
		// as always clickable
		return true;
	}

	else
	{
		return false;
	}
}

/*
i wondered why the value was not correct
and found out that the hidden, empty
balls on the fourth row are still clickable
*/

void Game::handleThirteen()
{
	// either OR spare/table touched last
	// and sum is 13
	if (totalThirteen())
	{
		// make sure we see the last highlight
		updateScreen();
		
		// kev
		SDL_EventState(
			SDL_FINGERUP, SDL_DISABLE);
		SDL_Delay(1000);
		SDL_EventState(
			SDL_FINGERUP, SDL_ENABLE);

		// spares has a currentSpare
		// no need for location vector
		if (table.spares[currentSpare].highlighted)
		{
			table.spares.erase(table.spares.begin() + currentSpare);
			--sparesLeft;
		}
		/*
            if vector index is 0, erase 0
            and index 1 becomes 0
            thats why we see the next ball
            without ++currentSpare

            if vector size is 9 we erase 9
            vector.erase() weirdly kept the end spare
            
            give the program a nudge
        */

		// if it was last spare in spares
		if (currentSpare >= sparesLeft)
			currentSpare = 0;

		// and table
		for (int x : locations)
		{
			// hide
			createEmpty(x);
			// deselect
			handleHighlight(x);
		}
		// tidy
		arrangeTotal();
	}
	// maybe arrange goes here?

	// check here?
	if (checkTableEmpty())
		beginBoard();
}

void Game::includeSpare()
{
	//kev
	if(table.spares[currentSpare].value != 0)
	{
	handleSpareHighlight(currentSpare);
	handleThirteen();
	}
}

void Game::loopSpares()
{
	if (sparesLeft == 0)
	{
		// when we reach the end of spares
		// it would be nice to see an end
		// and not loop back around
		// maybe an "end" ball?
		table.spares[currentSpare] =
			table.spares[SPARESMAXNUM-sparesLeft+1];
	}
	else
	{
		++currentSpare;
		// lucky "no index error"
		// will bump to 1
		// i dont get it

		// if prev was highlighted, not now
		if (table.spares[currentSpare - 1].highlighted)
		{
			// after incr deselect prev spare
			handleSpareHighlight(currentSpare - 1);
		}

		if (currentSpare > sparesLeft)
		{
			currentSpare = 0;
		}
	}
}

bool Game::totalThirteen()
{
	int total = 0;

	for (int t : totalSum)
		total += t;

	if (table.spares[currentSpare].highlighted)
		total += table.spares[currentSpare].value;

	// kev
	std::string tot_s = std::to_string(total);
	test_tex = loadText(renderer,
						test_font, tot_s.c_str());

	if (total == 13)
	{
		// i wonder why this breaks?
		//totalSum.clear();
		return true;
	}

	return false;
}

// returns the index number in locations
// these are locations 0 - 17
// if location 3 is highlighted first loc[0] = 3
int Game::findLocation(int value, std::vector<int> v)
{
	for (int i = 0; i < v.size(); ++i)
	{
		if (value == v[i])
			return i;
	}
	return -1;
}

/*
handleThirteen we use locations
to set balls w/h= 0 and show = false
*/

// sift out highlighted / unhighlighted
// although not including spares
void Game::arrangeTotal()
{
	for (int i = 0; i < table.balls.size(); ++i)
	{
		// do not push if ball location already present
		if (table.balls[i].highlighted)
		{
			int loc = findLocation(i, locations);
			// if there is no location
			if (loc == -1)
			{
				totalSum.push_back(table.balls[i].value);
				locations.push_back(i);
			}
		}

		if (!table.balls[i].highlighted)
		{
			int loc = findLocation(i, locations);
			// did find something
			if (loc > -1)
			{
				totalSum.erase(totalSum.begin() + loc);
				locations.erase(locations.begin() + loc);
			}
		}
	}
}

void Game::touchingBall(int i)
{
	if (canClick(i))
	{
		handleHighlight(i);
		arrangeTotal();
	}
	handleThirteen();
}

void Game::checkTime()
{
	// a bit like Lerp
	double elapsed = (time(0) - startTime);
	double length = SONG18LENGTH;
	double left = length - elapsed;
	bar = timer.w;

	bar *= (left / length);

	// do i --score here?
	// its to do with time, and bar 0
	// i see no other place that makes sense
	// if bar<=0
	// perhaps --score every frame
	// checkTime is every frame
	if (bar < 0)
	{
		bar = 0;
		if(score>0)
			--score;
	}

	timeRemaining.w = bar;
}

/*
keeping input short
i think it is tidy
*/
void Game::checkInput()
{
	while (SDL_PollEvent(&e) != 0)
	{
		//User touches screen
		if (e.type == SDL_FINGERUP)
		{
			f = e.tfinger;
			touch.x = f.x * sw;
			touch.y = f.y * sh;
			touch.w = 1;
			touch.h = 1;

			for (int i = 0; i < table.balls.size(); ++i)
			{
				// create Rect from Ball
				// kev
				/*
                SDL_Rect B{};
                B.x = table.balls[i].x;
                B.y = table.balls[i].y;
                B.w = table.balls[i].w;
                B.h = table.balls[i].h;
                */

				if (SDL_HasIntersection(
						&touch, &test_rects[i]))
				{
					touchingBall(i);
				}
			}

			if (SDL_HasIntersection(&touch, &reRackButton))
			{
				beginBoard();
			}

			if (SDL_HasIntersection(&touch, &spareButton))
			{
				loopSpares();
			}

			if (SDL_HasIntersection(&touch, &sparesPos))
			{
				includeSpare();
			}

			if (SDL_HasIntersection(&touch, &quitButton))
			{
				isQuit = true;
			}
		}
	}
}

void Game::updateScreen()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// first background
	SDL_RenderCopy(renderer, table.backgroundimg, NULL, &background);

	// pop a load of balls on screen
	for (int i = 0; i < table.balls.size(); ++i)
	{
		if (table.balls[i].show == true)
		{
			SDL_RenderCopy(
				renderer,
				table.balls[i].texture,
				NULL,
				&test_rects[i]);
		}
	}

	// displayscore
	SDL_RenderCopy(renderer, table.fonts[score].fontTexture, NULL, &scoreOnePos);

	// display current spare
	SDL_RenderCopy(renderer, table.spares[currentSpare].texture, NULL, &sparesPos);
	//
	// quit button
	//
	SDL_RenderCopy(renderer, table.quitimg, NULL, &quitButton);
	//
	// play again button
	//
	SDL_RenderCopy(renderer, table.reRackimg, NULL, &reRackButton);
	//
	// timer max/min
	//
	SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
	SDL_RenderFillRect(renderer, &timer);
	SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
	SDL_RenderFillRect(renderer, &timeRemaining);
	//
	// next spare
	//
	SDL_RenderCopy(renderer, table.turnballimg, NULL, &spareButton);

	//
	// debug
	// conditions
	//
	if (debug)
	{
		// kev
		SDL_RenderCopy(renderer,
					   test_tex, NULL, &test_total_rect);
		SDL_SetRenderDrawColor(
			renderer, 0, 0, 255, 255);
		for (int i = 0; i < test_rects.size(); ++i)
		{
			SDL_RenderDrawRect(
				renderer, &test_rects[i]);
		}
	}

	SDL_RenderPresent(renderer);
}