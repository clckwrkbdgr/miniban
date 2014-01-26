#pragma once
#include <QtCore/QMap>
#include <QtCore/QSize>
#include "abstractgamemode.h"
#include "sokoban.h"
#include "sprites.h"

class PlayingMode : public AbstractGameMode {
	Q_OBJECT
public:
	PlayingMode(const Sokoban & prepared_sokoban, const Sprites & sprites, QObject * parent = 0);
	virtual ~PlayingMode() {}

	const Sokoban & getCurrentSokoban() const { return sokoban; }
	
	virtual void invalidateRect();
	virtual void paint(SDL_Renderer * painter, const QRect & rect);
	virtual void processControl(int control);
signals:
	void levelIsSolved();
private:
	Sprites original_sprites;
	QSize spriteSize;
	bool toInvalidate;
	Sokoban sokoban;
	bool target_mode;
	QPoint target;

	void resizeSpritesForLevel(const QRect & rect);
};

