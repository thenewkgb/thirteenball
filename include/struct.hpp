#ifndef _STRUCT_HPP_
#define _STRUCT_HPP_
#include "SDL2/SDL.h"
#include <vector>
#endif

struct MyFont : SDL_Rect
{
	int value = 0;
	SDL_Texture *fontTexture = NULL;
};

struct Ball : SDL_Rect
{
	int value;
	bool show = true;
	bool highlighted = false;
	SDL_Texture *texture = NULL;

	// spares
	// if i'm still using colour tints
	// then remember colour
	int r = 0;
	int g = 0;
	int b = 0;
};

struct EmptyBall : SDL_Rect
{
	int value = 0;
};

struct Table
{
	std::vector<Ball> balls{};
	std::vector<Ball> spares{};
	std::vector<MyFont> fonts{};
	SDL_Texture *backgroundimg = nullptr;
	SDL_Texture *reRackimg = nullptr;
	SDL_Texture *quitimg = nullptr;
	SDL_Texture *turnballimg = nullptr;
	SDL_Texture *scoreDigitOne = nullptr;
};