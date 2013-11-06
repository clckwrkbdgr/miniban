#include <QtDebug>
#include <QtGui/QPainter>
#include "sokoban.h"
#include "sprites.h"
#include "playingmode.h"

namespace { // Aux.

const int MIN_SCALE_FACTOR = 1;
const int MAX_SCALE_FACTOR = 8;

const QList<int> & getAllTileTypes()
{
	static QList<int> tileTypes = QList<int>()
		<< Sokoban::FLOOR
		<< Sokoban::WALL
		<< Sokoban::EMPTY_SLOT
		<< Sokoban::SPACE
		<< Sokoban::PLAYER_ON_FLOOR
		<< Sokoban::PLAYER_ON_SLOT
		<< Sokoban::BOX_ON_FLOOR
		<< Sokoban::BOX_ON_SLOT;
	return tileTypes;
}

};

PlayingMode::PlayingMode(const QString & level, QObject * parent)
	: AbstractGameMode(parent), originalLevel(level), toInvalidate(true), sokoban(level)
{
}

void PlayingMode::resizeSpritesForLevel(const QRect & rect)
{
	QSize originalSize = Sprites::getSpritesBounds();
	int scaleFactor = qMin(
			rect.width() / (sokoban.width() * originalSize.width()),
			rect.height() / (sokoban.height() * originalSize.height())
			);
	scaleFactor = qBound(MIN_SCALE_FACTOR, scaleFactor, MAX_SCALE_FACTOR);

	foreach(int tileType, getAllTileTypes()) {
		sprites[tileType] = Sprites::getSprite(tileType, scaleFactor);
	}

	spriteSize = originalSize * scaleFactor;
}

void PlayingMode::processControl(int control)
{
	switch(control) {
		case CONTROL_LEFT:  sokoban.movePlayer(Sokoban::LEFT); break;
		case CONTROL_DOWN:  sokoban.movePlayer(Sokoban::DOWN); break;
		case CONTROL_UP:    sokoban.movePlayer(Sokoban::UP); break;
		case CONTROL_RIGHT: sokoban.movePlayer(Sokoban::RIGHT); break;
		case CONTROL_RUN_LEFT:  sokoban.runPlayer(Sokoban::LEFT); break;
		case CONTROL_RUN_DOWN:  sokoban.runPlayer(Sokoban::DOWN); break;
		case CONTROL_RUN_UP:    sokoban.runPlayer(Sokoban::UP); break;
		case CONTROL_RUN_RIGHT: sokoban.runPlayer(Sokoban::RIGHT); break;
		case CONTROL_HOME: sokoban = Sokoban(originalLevel); break;
		case CONTROL_UNDO:
				   sokoban.undo();
				   break;
		default: return;
	}
	if(sokoban.isSolved()) {
		emit levelIsSolved();
	}
}

void PlayingMode::invalidateRect()
{
	toInvalidate = true;
}

void PlayingMode::paint(QPainter * painter, const QRect & rect)
{
	if(toInvalidate) {
		resizeSpritesForLevel(rect);
		toInvalidate = false;
	}

	painter->fillRect(rect, Qt::black);

	QPoint offset = QPoint(
			rect.width() - sokoban.width() * spriteSize.width(),
			rect.height() - sokoban.height() * spriteSize.height()
			) / 2;
	for(int y = 0; y < sokoban.height(); ++y) {
		for(int x = 0; x < sokoban.width(); ++x) {
			QPoint pos = offset + QPoint(x * spriteSize.width(), y * spriteSize.height());
			bool validSprite = sprites.contains(sokoban.getCell(QPoint(x, y)));
			QChar tileType = validSprite ? sokoban.getCell(QPoint(x, y)) : QChar(Sokoban::FLOOR);
			painter->drawImage(pos, sprites[tileType]);
		}
	}
}


