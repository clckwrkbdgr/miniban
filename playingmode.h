#pragma once
#include <QtCore/QMap>
#include <QtCore/QSize>
#include "sokoban.h"
#include "sprites.h"

class Game {
public:
	enum {
		CONTROL_NONE, CONTROL_SKIP,
		CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN,
		CONTROL_UP_LEFT, CONTROL_UP_RIGHT, CONTROL_DOWN_LEFT, CONTROL_DOWN_RIGHT,
		CONTROL_RUN_LEFT, CONTROL_RUN_RIGHT, CONTROL_RUN_UP, CONTROL_RUN_DOWN,
		CONTROL_TARGET, CONTROL_GOTO,
		CONTROL_UNDO, CONTROL_HOME, CONTROL_QUIT,
		CONTROL_CHEAT_RESTART, CONTROL_CHEAT_SKIP_LEVEL
	} Control;

	Game(const Sokoban & prepared_sokoban, const Sprites & sprites);
	virtual ~Game() {}

	void load(const Sokoban & prepared_sokoban);
	virtual void paint(SDL_Renderer * painter, const QRect & rect);
	virtual void processControl(int control);
	virtual bool is_done() const;
	void processTime(int msec_passed);
private:
	const Sprites & original_sprites;
	QSize spriteSize;
	bool toInvalidate;
	Sokoban sokoban;
	bool target_mode;
	QPoint target;

	bool fading;
	int current_fade, max_fade;

	void resizeSpritesForLevel(const QRect & rect);
};

