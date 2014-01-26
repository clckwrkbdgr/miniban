#pragma once
#include <QtCore/QRect>
#include <QtCore/QMap>
class SDL_Texture;
class SDL_Renderer;

class Sprites {
public:
	Sprites() : tileset(0) {}

	enum { FLOOR, WALL, EMPTY_SLOT, SPACE, PLAYER_ON_FLOOR, PLAYER_ON_SLOT, BOX_ON_FLOOR, BOX_ON_SLOT, CURSOR };
	void init(SDL_Renderer * renderer);
	QRect getSpriteRect(int tileType, int tileIndex) const;
	SDL_Texture * getTileSet() const;
	QSize getSpritesBounds() const;
	bool contains(int tileType) const;
private:
	SDL_Texture * tileset;
	QSize sprite_size;
	QMap<int, QList<QPoint> > cachedSprites;
};
