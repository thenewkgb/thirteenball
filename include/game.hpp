#ifndef _GAME_HPP_
#define _GAME_HPP_
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#include <vector>

#include "struct.hpp"
#endif

constexpr int SPARESMAXNUM = 10;
constexpr int SONG18LENGTH = 50;

class Game
{
	bool debug = false;

  public:
	Game()
	{
		// debug
		test_rects.resize(18);
	}

	// sdl
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Event e{};
	SDL_TouchFingerEvent f;

	// mixer
	Mix_Music *music1 = NULL;
	Mix_Music *music2 = NULL;
	Mix_Music *music3 = NULL;
	Mix_Music *music4 = NULL;

	Mix_Chunk *click = NULL;
	Mix_Chunk *win = NULL;

	SDL_Rect quitButton{};
	SDL_Rect reRackButton{};
	SDL_Rect spareButton{};
	SDL_Rect sparesPos{};
	SDL_Rect touch{};
	SDL_Rect background{};
	SDL_Rect scoreOnePos{};

	// kev
	SDL_Rect test_ball_size{};
	std::vector<SDL_Rect> test_rects;
	int test_total = 0;
	SDL_Rect test_total_rect{};
	TTF_Font *test_font = NULL;
	SDL_Texture *test_tex = NULL;

	SDL_Rect timer{};
	SDL_Rect timeRemaining{};

	Table table{};

	// some miscellanious vars
	int sw, sh;
	unsigned int currentSpare = 0;
	unsigned int sparesLeft = SPARESMAXNUM;
	bool isQuit{};
	unsigned int songNo = 0;
	unsigned int timeLeft = SONG18LENGTH;
	unsigned int startTime = 0;
	unsigned int score = 0;
	unsigned int ballSize = 0;
	// kev
	double bar = 0; // used for scoring

	std::vector<int> totalSum{};
	std::vector<int> locations{};

	int init();
	void quit();
	void setupTable();
	void loadGFX();
	int loadGameSFX();
	int loadGameMusic();

	void highlight(int i);
	void deselect(int i);
	void handleHighlight(int i);
	void hightlightSpare(int spareNum);
	void deselectSpare(int spareNum);
	void handleSpareHighlight(int spareNum);

	void createEmpty(int i);
	void touchingBall(int i);
	bool canClick(int i);
	void arrangeTotal();
	void replaceEmpty();
	int findLocation(int value, std::vector<int> v);

	bool totalThirteen();
	void handleThirteen();
	bool checkTableEmpty();

	void loopSpares();
	void colourSpares();
	void includeSpare();
	void colourBalls();

	void chooseSong();
	void checkTime();
	void updateUI();

	void clearValues();
	void beginBoard();
	void updateScreen();
	void checkInput();
};