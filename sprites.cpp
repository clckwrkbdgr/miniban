#include <QtDebug>
#include "sprites.h"
#include "sokoban.h"
#include <QtCore/QMap>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <chthon/pixmap.h>
#include <chthon/util.h>
#include <SDL2/SDL.h>

namespace Sprite {
#include "sokoban.xpm"
}

void Sprites::init(SDL_Renderer * renderer)
{
	try {
		std::vector<std::string> sokoban_lines(Sprite::sokoban, Sprite::sokoban + Chthon::size_of_array(Sprite::sokoban));
		Chthon::Pixmap pixmap(sokoban_lines);
		SDL_Surface * sokoban_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				pixmap.width(), pixmap.height(), 32,
				0x00ff0000,
				0x0000ff00,
				0x000000ff,
				0xff000000
				);
		if(SDL_MUSTLOCK(sokoban_surface)) {
			SDL_LockSurface(sokoban_surface);
		}
		for(unsigned x = 0; x < pixmap.width(); ++x) {
			for(unsigned y = 0; y < pixmap.height(); ++y) {
				Uint8 * pixel = (Uint8*)sokoban_surface->pixels;
				pixel += (y * sokoban_surface->pitch) + (x * sizeof(Uint32));
				Uint32 c = pixmap.color(pixmap.pixel(x, y)).argb();
				*((Uint32*)pixel) = c;
			}
		}
		if(SDL_MUSTLOCK(sokoban_surface)) {
			SDL_UnlockSurface(sokoban_surface);
		}
		tileset = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, pixmap.width(), pixmap.height());
		SDL_UpdateTexture(tileset, 0, sokoban_surface->pixels, sokoban_surface->pitch);
		SDL_SetTextureBlendMode(tileset, SDL_BLENDMODE_BLEND);
	} catch(const Chthon::Pixmap::Exception & e) {
		QTextStream err(stderr);
		err << QString::fromStdString(e.what) << endl;
	}

	if(tileset == 0) {
		return;
	}
	cachedSprites[Sprites::FLOOR]           << QPoint(0, 0) << QPoint(1, 0) << QPoint(2, 0) << QPoint(3, 0);
	cachedSprites[Sprites::WALL]            << QPoint(0, 1) << QPoint(1, 1) << QPoint(2, 1) << QPoint(3, 1);
	cachedSprites[Sprites::EMPTY_SLOT]      << QPoint(0, 2) << QPoint(1, 2) << QPoint(2, 2) << QPoint(3, 2);
	cachedSprites[Sprites::SPACE]           << QPoint(0, 3) << QPoint(1, 3) << QPoint(2, 3) << QPoint(3, 3);
	cachedSprites[Sprites::PLAYER_ON_FLOOR] << QPoint(0, 4) << QPoint(1, 4) << QPoint(2, 4) << QPoint(3, 4);
	cachedSprites[Sprites::PLAYER_ON_SLOT]  << QPoint(0, 5) << QPoint(1, 5) << QPoint(2, 5) << QPoint(3, 5);
	cachedSprites[Sprites::BOX_ON_FLOOR]    << QPoint(0, 6) << QPoint(1, 6) << QPoint(2, 6) << QPoint(3, 6);
	cachedSprites[Sprites::BOX_ON_SLOT]     << QPoint(0, 7) << QPoint(1, 7) << QPoint(2, 7) << QPoint(3, 7);
	cachedSprites[Sprites::CURSOR]          << QPoint(0, 8);
	int max_x = 0, max_y = 0;
	for(int key : cachedSprites.keys()) {
		for(const QPoint & point : cachedSprites[key]) {
			max_x = qMax(point.x(), max_x);
			max_y = qMax(point.y(), max_y);
		}
	}
	int w, h;
	SDL_QueryTexture(tileset, 0, 0, &w, &h);
	sprite_size = QSize(w / (max_x + 1), h / (max_y + 1));
}

QSize Sprites::getSpritesBounds() const
{
	if(tileset == 0) {
		return QSize();
	}
	return sprite_size;
}

QRect Sprites::getSpriteRect(int tileType, int spriteIndex) const
{
	if(cachedSprites.contains(tileType)) {
		QPoint tile_pos = cachedSprites[tileType].value(spriteIndex);
		return QRect(QPoint(tile_pos.x() * sprite_size.width(), tile_pos.y() * sprite_size.height()), sprite_size);
	}
	return QRect();
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
	return cachedSprites.contains(tileType);
}
