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

PlayingMode::PlayingMode(const QString & level, const Sprites & _sprites, QObject * parent)
	: AbstractGameMode(parent), originalLevel(level), original_sprites(_sprites), toInvalidate(true), sokoban(level), target_mode(false)
{
}

void PlayingMode::resizeSpritesForLevel(const QRect & rect)
{
	QSize originalSize = original_sprites.getSpritesBounds();
	int scaleFactor = qMin(
			rect.width() / (sokoban.width() * originalSize.width()),
			rect.height() / (sokoban.height() * originalSize.height())
			);
	scaleFactor = qBound(MIN_SCALE_FACTOR, scaleFactor, MAX_SCALE_FACTOR);

	foreach(int tileType, getAllTileTypes()) {
		sprites[tileType] = original_sprites.getSprite(tileType, scaleFactor);
	}

	spriteSize = originalSize * scaleFactor;

	aim = QImage(spriteSize, sprites[Sokoban::FLOOR].format());
	QPainter painter(&aim);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(aim.rect(), Qt::blue);
	painter.fillRect(scaleFactor, scaleFactor, spriteSize.width() - scaleFactor * 2, spriteSize.height() - scaleFactor * 2, Qt::transparent);
}

void PlayingMode::processControl(int control)
{
	if(target_mode) {
		QPoint new_target = target;
		switch(control) {
			case CONTROL_LEFT:  new_target.rx()--; break;
			case CONTROL_DOWN:  new_target.ry()++; break;
			case CONTROL_UP:    new_target.ry()--; break;
			case CONTROL_RIGHT: new_target.rx()++; break;
			case CONTROL_UP_LEFT: new_target += QPoint(-1, -1); break;
			case CONTROL_UP_RIGHT: new_target += QPoint(1, -1); break;
			case CONTROL_DOWN_LEFT: new_target += QPoint(-1, 1); break;
			case CONTROL_DOWN_RIGHT: new_target += QPoint(1, 1); break;
			case CONTROL_GOTO:
				sokoban.movePlayer(target);
				target_mode = false;
				break;
			case CONTROL_TARGET:  target_mode = false; break;
			default: break;
		}
		if(sokoban.isValid(new_target)) {
			target = new_target;
		}
		return;
	}
	switch(control) {
		case CONTROL_LEFT:  sokoban.movePlayer(Sokoban::LEFT); break;
		case CONTROL_DOWN:  sokoban.movePlayer(Sokoban::DOWN); break;
		case CONTROL_UP:    sokoban.movePlayer(Sokoban::UP); break;
		case CONTROL_RIGHT: sokoban.movePlayer(Sokoban::RIGHT); break;
		case CONTROL_RUN_LEFT:  sokoban.runPlayer(Sokoban::LEFT); break;
		case CONTROL_RUN_DOWN:  sokoban.runPlayer(Sokoban::DOWN); break;
		case CONTROL_RUN_UP:    sokoban.runPlayer(Sokoban::UP); break;
		case CONTROL_RUN_RIGHT: sokoban.runPlayer(Sokoban::RIGHT); break;
		case CONTROL_TARGET:
			target_mode = true;
			target = sokoban.getPlayerPos();
			break;
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
			int cellSprite = sokoban.getCellSprite(QPoint(x, y));
			painter->drawImage(pos, sprites[sprites.contains(cellSprite) ? cellSprite : Sokoban::SPACE]);
			int objectSprite = sokoban.getObjectSprite(QPoint(x, y));
			if(objectSprite != Sokoban::NONE && sprites.contains(objectSprite)) {
				painter->drawImage(pos, sprites[objectSprite]);
			}
		}
	}
	if(target_mode) {
		QPoint pos = offset + QPoint(target.x() * spriteSize.width(), target.y() * spriteSize.height());
		painter->drawImage(pos, aim);
	}
}


