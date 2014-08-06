#pragma once
#include "sokoban.h"
#include "sprites.h"
#include "counter.h"
class SDL_Rect;

class Game {
public:
	enum {
		CONTROL_NONE, CONTROL_SKIP,
		CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN,
		CONTROL_UP_LEFT, CONTROL_UP_RIGHT, CONTROL_DOWN_LEFT, CONTROL_DOWN_RIGHT,
		CONTROL_RUN_LEFT, CONTROL_RUN_RIGHT, CONTROL_RUN_UP, CONTROL_RUN_DOWN,
		CONTROL_TARGET, CONTROL_GOTO,
		CONTROL_UNDO, CONTROL_HOME, CONTROL_QUIT,
		CONTROL_ESCAPE,
		CONTROL_CHEAT_RESTART, CONTROL_CHEAT_SKIP_LEVEL
	} Control;

	Game(const Sokoban & prepared_sokoban, const Sprites & sprites);
	virtual ~Game() {}

	void load(const Sokoban & prepared_sokoban);
	virtual void paint(SDL_Renderer * painter, const SDL_Rect & rect);
	virtual void processControl(int control);
	virtual bool is_done() const;
	void processTime(int msec_passed);
private:
	const Sprites & original_sprites;
	int sprite_width;
	int sprite_height;
	bool toInvalidate;
	Sokoban sokoban;
	bool target_mode;
	Chthon::Point target;

	Counter fader_in;
	Counter fader_out;

	void resizeSpritesForLevel(const SDL_Rect & rect);
};

