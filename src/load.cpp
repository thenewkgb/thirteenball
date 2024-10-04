#include "load.hpp"



SDL_Texture *loadImage(SDL_Renderer *renderer, const char *path)
{
	SDL_Surface *img = IMG_Load(path);
	if (img == NULL)
	{
		return NULL;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);

	SDL_FreeSurface(img);

	if (texture == NULL)
	{
		return NULL;
	}
	return texture;
}

SDL_Texture *loadText(
	SDL_Renderer *renderer,
	TTF_Font *font,
	const char *text)
{
	SDL_Color font_col = {255, 255, 255};

	SDL_Surface *font_surface =
		TTF_RenderText_Solid(
			font, text, font_col);

	SDL_Texture *tex =
		SDL_CreateTextureFromSurface(
			renderer, font_surface);
	SDL_FreeSurface(font_surface);

	if (tex == NULL)
		return NULL;

	return tex;
}

Mix_Music *loadMusic(const char *path)
{
	Mix_Music *music = Mix_LoadMUS(path);
	if (music == NULL)
	{
		return NULL;
	}
	return music;
}

Mix_Chunk *loadSound(const char *path)
{
	Mix_Chunk *sound = Mix_LoadWAV(path);
	if (sound == NULL)
		return NULL;

	return sound;
}