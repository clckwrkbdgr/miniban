#include "sprites.h"
#include "sokoban.h"
#include <chthon/pixmap.h>
#include <chthon/util.h>
#include <SDL2/SDL.h>
#include <iostream>
using namespace Chthon;

namespace Sprite {
#include "sokoban.xpm"
#include "font.xpm"
SDL_Texture * load(SDL_Renderer * renderer, const char ** xpm, int size);
}

SDL_Texture * Sprite::load(SDL_Renderer * renderer, const char ** xpm, int size)
{
	SDL_Texture * result = 0;
	try {
		std::vector<std::string> xpm_lines(xpm, xpm + size);
		Chthon::Pixmap pixmap(xpm_lines);
		SDL_Surface * surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				pixmap.width(), pixmap.height(), 32,
				0x00ff0000,
				0x0000ff00,
				0x000000ff,
				0xff000000
				);
		if(SDL_MUSTLOCK(surface)) {
			SDL_LockSurface(surface);
		}
		for(unsigned x = 0; x < pixmap.width(); ++x) {
			for(unsigned y = 0; y < pixmap.height(); ++y) {
				Uint8 * pixel = (Uint8*)surface->pixels;
				pixel += (y * surface->pitch) + (x * sizeof(Uint32));
				Uint32 c = pixmap.color(pixmap.pixel(x, y)).argb();
				*((Uint32*)pixel) = c;
			}
		}
		if(SDL_MUSTLOCK(surface)) {
			SDL_UnlockSurface(surface);
		}
		result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, pixmap.width(), pixmap.height());
		SDL_UpdateTexture(result, 0, surface->pixels, surface->pitch);
		SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND);
	} catch(const Chthon::Pixmap::Exception & e) {
		std::cerr << e.what << std::endl;
	}
	return result;
}

void Sprites::init(SDL_Renderer * renderer)
{
	tileset = Sprite::load(renderer, Sprite::sokoban, Chthon::size_of_array(Sprite::sokoban));
	font = Sprite::load(renderer, Sprite::font, Chthon::size_of_array(Sprite::font));
	
	cachedSprites[Sprites::FLOOR]           << Chthon::Point(0, 0) << Chthon::Point(1, 0) << Chthon::Point(2, 0) << Chthon::Point(3, 0);
	cachedSprites[Sprites::WALL]            << Chthon::Point(0, 1) << Chthon::Point(1, 1) << Chthon::Point(2, 1) << Chthon::Point(3, 1);
	cachedSprites[Sprites::EMPTY_SLOT]      << Chthon::Point(0, 2) << Chthon::Point(1, 2) << Chthon::Point(2, 2) << Chthon::Point(3, 2);
	cachedSprites[Sprites::SPACE]           << Chthon::Point(0, 3) << Chthon::Point(1, 3) << Chthon::Point(2, 3) << Chthon::Point(3, 3);
	cachedSprites[Sprites::PLAYER_ON_FLOOR] << Chthon::Point(0, 4) << Chthon::Point(1, 4) << Chthon::Point(2, 4) << Chthon::Point(3, 4);
	cachedSprites[Sprites::PLAYER_ON_SLOT]  << Chthon::Point(0, 5) << Chthon::Point(1, 5) << Chthon::Point(2, 5) << Chthon::Point(3, 5);
	cachedSprites[Sprites::BOX_ON_FLOOR]    << Chthon::Point(0, 6) << Chthon::Point(1, 6) << Chthon::Point(2, 6) << Chthon::Point(3, 6);
	cachedSprites[Sprites::BOX_ON_SLOT]     << Chthon::Point(0, 7) << Chthon::Point(1, 7) << Chthon::Point(2, 7) << Chthon::Point(3, 7);
	cachedSprites[Sprites::CURSOR]          << Chthon::Point(0, 8);
	int max_x = 0, max_y = 0;
	for(auto & key_value : cachedSprites) {
		for(const Point & point : key_value.second) {
			max_x = std::max(point.x, max_x);
			max_y = std::max(point.y, max_y);
		}
	}
	int w, h;
	SDL_QueryTexture(tileset, 0, 0, &w, &h);
	sprite_width = w / (max_x + 1);
	sprite_height = h / (max_y + 1);
}

SDL_Rect Sprites::getSpritesBounds() const
{
	SDL_Rect result = {0, 0, 0, 0};
	if(tileset == 0) {
		return result;
	}
	result.w = sprite_width;
	result.h = sprite_height;
	return result;;
}

SDL_Rect Sprites::getCharRect(char ch) const
{
	SDL_Rect result;
	result.x = ch % 16 * 20;
	result.y = ch / 16 * 20;
	result.w = 20;
	result.h = 20;
	return result;
}

SDL_Texture * Sprites::getFont() const
{
	return font;
}

SDL_Rect Sprites::getSpriteRect(int tileType, int spriteIndex) const
{
	SDL_Rect result = {0, 0, 0, 0};
	if(cachedSprites.count(tileType) > 0) {
		Point tile_pos = cachedSprites.find(tileType)->second[spriteIndex];
		result.x = tile_pos.x * sprite_width;
		result.y = tile_pos.y * sprite_height;
		result.w = sprite_width;
		result.h = sprite_height;
		return result;
	}
	return result;
}

SDL_Texture * Sprites::getTileSet() const
{
	return tileset;
}

bool Sprites::contains(int tileType) const
{
	if(tileset == 0) {
		return false;
	}
	return cachedSprites.count(tileType) > 0;
}
