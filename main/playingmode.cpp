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
		<< Sokoban::TileType::FLOOR
		<< Sokoban::TileType::WALL
		<< Sokoban::TileType::EMPTY_SLOT
		<< Sokoban::TileType::PLAYER_ON_FLOOR
		<< Sokoban::TileType::PLAYER_ON_SLOT
		<< Sokoban::TileType::BOX_ON_FLOOR
		<< Sokoban::TileType::BOX_ON_SLOT;
	return tileTypes;
}

QSize calculateLevelSize(const QString & level)
{
	int levelWidth = 0;
	int levelHeight = 1;
	int lineWidth = 0;
	foreach(const QChar & c, level) {
		if(c == '\n') {
			++levelHeight;
			if(levelWidth < lineWidth) {
				levelWidth = lineWidth;
			}
			lineWidth = 0;
		} else {
			++lineWidth;
		}
	}
	return QSize(levelWidth, levelHeight);
}

};

PlayingMode::PlayingMode(const QString & level, QObject * parent)
	: AbstractGameMode(parent), toInvalidate(true)
{
	restartLevel(level);
}

void PlayingMode::resizeSpritesForLevel(const QSize & levelSize, const QRect & rect)
{
	QSize originalSize = Sprites::getSpritesBounds();
	int scaleFactor = qMin(
			rect.width() / (levelSize.width() * originalSize.width()),
			rect.height() / (levelSize.height() * originalSize.height())
			);
	scaleFactor = qBound(MIN_SCALE_FACTOR, scaleFactor, MAX_SCALE_FACTOR);

	foreach(int tileType, getAllTileTypes()) {
		sprites[tileType] = Sprites::getSprite(tileType, scaleFactor);
	}

	spriteSize = originalSize * scaleFactor;
}

void PlayingMode::restartLevel(const QString & level)
{
	originalLevel = level;
	currentLevel = originalLevel;
	history.clear();
	currentLevelSize = calculateLevelSize(currentLevel);
	invalidateRect();
}

void PlayingMode::processControl(int control)
{
	switch(control) {
		case CONTROL_SKIP:  emit levelIsSolved(); return;
		case CONTROL_LEFT:  currentLevel = Sokoban::process(currentLevel, Sokoban::Control::LEFT, &history); break;
		case CONTROL_DOWN:  currentLevel = Sokoban::process(currentLevel, Sokoban::Control::DOWN, &history); break;
		case CONTROL_UP:    currentLevel = Sokoban::process(currentLevel, Sokoban::Control::UP, &history); break;
		case CONTROL_RIGHT: currentLevel = Sokoban::process(currentLevel, Sokoban::Control::RIGHT, &history); break;
		case CONTROL_HOME: restartLevel(originalLevel); break;
		case CONTROL_UNDO:
				   if(!history.isEmpty()) {
					   currentLevel = Sokoban::undo(currentLevel, &history);
				   }
				   break;
		default: return;
	}
	if(Sokoban::isSolved(currentLevel)) {
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
		resizeSpritesForLevel(currentLevelSize, rect);
		toInvalidate = false;
	}

	painter->fillRect(rect, Qt::black);
	QStringList rows = currentLevel.split('\n');

	QPoint offset = QPoint(
			rect.width() - currentLevelSize.width() * spriteSize.width(),
			rect.height() - currentLevelSize.height() * spriteSize.height()
			) / 2;
	for(int y = 0; y < currentLevelSize.height(); ++y) {
		for(int x = 0; x < currentLevelSize.width(); ++x) {
			QPoint pos = offset + QPoint(x * spriteSize.width(), y * spriteSize.height());
			bool validSprite = sprites.contains(rows[y][x]);
			bool stillInRow = x < rows[y].length();
			QChar tileType = (validSprite && stillInRow) ? rows[y][x] : QChar(Sokoban::TileType::FLOOR);
			painter->drawImage(pos, sprites[tileType]);
		}
	}
}


