#pragma once
#include "levelset.h"
#include "sprites.h"
class SDL_KeyboardEvent;
class SDL_Renderer;
class AbstractGameMode;

class SokobanWidget {
public:
	SokobanWidget();
	virtual ~SokobanWidget();
	int exec();
protected:
	int keyToControl(SDL_KeyboardEvent * event);
private:
	SDL_Renderer * renderer;
	SDL_Texture * snapshot;
	LevelSet levelSet;
	Sprites sprites;
	bool quit;
	QRect rect;
};

