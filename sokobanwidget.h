#pragma once
#include "levelset.h"
#include "sprites.h"
#include "settings.h"
class SDL_KeyboardEvent;
class SDL_Renderer;

class SokobanWidget {
public:
	SokobanWidget(int argc, char ** argv);
	virtual ~SokobanWidget();
	int exec();
protected:
	int keyToControl(SDL_KeyboardEvent * event);
private:
	Settings settings;
	SDL_Renderer * renderer;
	SDL_Texture * snapshot;
	LevelSet levelSet;
	Sprites sprites;
	bool quit;
	QRect rect;
};

