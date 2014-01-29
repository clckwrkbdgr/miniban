#pragma once
#include <map>
#include <vector>
class SDL_Texture;
class SDL_Renderer;
class SDL_Rect;
namespace Chthon { class Point; }

class Sprites {
public:
	Sprites() : tileset(0), font(0) {}

	enum { FLOOR, WALL, EMPTY_SLOT, SPACE, PLAYER_ON_FLOOR, PLAYER_ON_SLOT, BOX_ON_FLOOR, BOX_ON_SLOT, CURSOR };
	void init(SDL_Renderer * renderer);
	SDL_Rect getSpriteRect(int tileType, int tileIndex) const;
	SDL_Texture * getTileSet() const;
	SDL_Rect getCharRect(char ch) const;
	SDL_Texture * getFont() const;
	SDL_Rect getSpritesBounds() const;
	bool contains(int tileType) const;
private:
	SDL_Texture * tileset;
	SDL_Texture * font;
	int sprite_width, sprite_height;
	std::map<int, std::vector<Chthon::Point> > cachedSprites;
};
