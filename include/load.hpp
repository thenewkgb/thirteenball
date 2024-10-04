#ifndef _LOAD_HPP_
#define _LOAD_HPP_
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#endif

SDL_Texture *loadImage(SDL_Renderer *renderer, const char *path);

SDL_Texture *loadText(SDL_Renderer *renderer, TTF_Font *font, const char *text);

Mix_Music *loadMusic(const char *path);

Mix_Chunk *loadSound(const char *path);