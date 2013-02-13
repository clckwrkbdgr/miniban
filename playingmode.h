#pragma once
#include <QtCore/QMap>
#include <QtCore/QSize>
#include <QtGui/QImage>
#include "abstractgamemode.h"
#include "sokoban.h"

class PlayingMode : public AbstractGameMode {
	Q_OBJECT
public:
	PlayingMode(const QString & level, QObject * parent = 0);
	virtual ~PlayingMode() {}

	const QString & getCurrentLevel() const { return currentLevel; }
	
	virtual void invalidateRect();
	virtual void paint(QPainter * painter, const QRect & rect);
	virtual void processControl(int control);
signals:
	void levelIsSolved();
private:
	QString originalLevel;
	QString currentLevel;
	QSize currentLevelSize;
	QString history;
	QMap<QChar, QImage> sprites;
	QSize spriteSize;
	bool toInvalidate;
	Sokoban sokoban;

	void resizeSpritesForLevel(const QSize & levelSize, const QRect & rect);
};

