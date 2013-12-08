#pragma once
#include <QtCore/QMap>
#include <QtCore/QSize>
#include <QtGui/QImage>
#include "abstractgamemode.h"
#include "sokoban.h"
#include "sprites.h"

class PlayingMode : public AbstractGameMode {
	Q_OBJECT
public:
	PlayingMode(const QString & level, const Sprites & sprites, QObject * parent = 0);
	virtual ~PlayingMode() {}

	QString getCurrentLevel() const { return sokoban.toString(); }
	
	virtual void invalidateRect();
	virtual void paint(QPainter * painter, const QRect & rect);
	virtual void processControl(int control);
signals:
	void levelIsSolved();
private:
	QString originalLevel;
	Sprites original_sprites;
	QSize spriteSize;
	bool toInvalidate;
	Sokoban sokoban;
	bool target_mode;
	QPoint target;
	QImage aim;

	void resizeSpritesForLevel(const QRect & rect);
};

