#include <QtDebug>
#include <QtGui/QPainter>
#include "sokoban.h"
#include "sprites.h"
#include "playingmode.h"

const int MIN_SCALE_FACTOR = 1;
const int MAX_SCALE_FACTOR = 8;

PlayingMode::PlayingMode(const Sokoban & prepared_sokoban, const Sprites & _sprites, QObject * parent)
	: AbstractGameMode(parent), original_sprites(_sprites), toInvalidate(true),
	sokoban(prepared_sokoban), target_mode(false)
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

	spriteSize = originalSize * scaleFactor;

	aim = QImage(spriteSize, original_sprites.getTileSet().format());
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
				sokoban.movePlayer(Chthon::Point(target.x(), target.y()));
				target_mode = false;
				break;
			case CONTROL_TARGET:  target_mode = false; break;
			default: break;
		}
		if(sokoban.isValid(Chthon::Point(new_target.x(), new_target.y()))) {
			target = new_target;
		}
		return;
	}
	switch(control) {
		case CONTROL_LEFT:  sokoban.movePlayer(Sokoban::LEFT); break;
		case CONTROL_DOWN:  sokoban.movePlayer(Sokoban::DOWN); break;
		case CONTROL_UP:    sokoban.movePlayer(Sokoban::UP); break;
		case CONTROL_RIGHT: sokoban.movePlayer(Sokoban::RIGHT); break;
		case CONTROL_UP_LEFT:    sokoban.movePlayer(Sokoban::UP_LEFT); break;
		case CONTROL_UP_RIGHT:   sokoban.movePlayer(Sokoban::UP_RIGHT); break;
		case CONTROL_DOWN_LEFT:  sokoban.movePlayer(Sokoban::DOWN_LEFT); break;
		case CONTROL_DOWN_RIGHT: sokoban.movePlayer(Sokoban::DOWN_RIGHT); break;
		case CONTROL_RUN_LEFT:  sokoban.runPlayer(Sokoban::LEFT); break;
		case CONTROL_RUN_DOWN:  sokoban.runPlayer(Sokoban::DOWN); break;
		case CONTROL_RUN_UP:    sokoban.runPlayer(Sokoban::UP); break;
		case CONTROL_RUN_RIGHT: sokoban.runPlayer(Sokoban::RIGHT); break;
		case CONTROL_TARGET: {
			target_mode = true;
			Chthon::Point player = sokoban.getPlayerPos();
			target = QPoint(player.x, player.y);
			break;
		}
		case CONTROL_HOME: sokoban.restart(); break;
		case CONTROL_UNDO:
			try {
				sokoban.undo();
			} catch(const Sokoban::InvalidUndoException & e) {
				QTextStream err(stderr);
				err << tr("Invalid undo: %1. History string: '%2'").arg(e.invalidUndoControl).arg(QString::fromStdString(sokoban.historyAsString())) << endl;
			}
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

			Cell cell = sokoban.getCellAt(x, y);
			int cellSprite = Sprites::SPACE;
			switch(cell.type) {
				case Cell::FLOOR: cellSprite = Sprites::FLOOR; break;
				case Cell::SLOT: cellSprite = Sprites::EMPTY_SLOT; break;
				case Cell::SPACE: cellSprite = Sprites::SPACE; break;
				case Cell::WALL: cellSprite = Sprites::WALL; break;
			}
			cellSprite = original_sprites.contains(cellSprite) ? cellSprite : Sprites::SPACE;
			painter->drawImage(pos,
					original_sprites.getTileSet().copy(original_sprites.getSpriteRect(cellSprite, cell.sprite)).scaled(spriteSize)
					);

			Object object = sokoban.getObjectAt(x, y);
			if(!object.isNull()) {
				int objectSprite = Sprites::SPACE;
				switch(cell.type) {
					case Cell::FLOOR:
						if(object.is_player) {
							objectSprite = Sprites::PLAYER_ON_FLOOR;
						} else {
							objectSprite = Sprites::BOX_ON_FLOOR;
						}
						break;
					case Cell::SLOT:
						if(object.is_player) {
							objectSprite = Sprites::PLAYER_ON_SLOT;
						} else {
							objectSprite = Sprites::BOX_ON_SLOT;
						}
						break;
				}
				if(objectSprite != Sprites::SPACE && original_sprites.contains(objectSprite)) {
					painter->drawImage(pos,
							original_sprites.getTileSet().copy(original_sprites.getSpriteRect(objectSprite, object.sprite)).scaled(spriteSize)
							);
				}
			}
		}
	}
	if(target_mode) {
		QPoint pos = offset + QPoint(target.x() * spriteSize.width(), target.y() * spriteSize.height());
		painter->drawImage(pos,
				original_sprites.getTileSet().copy(original_sprites.getSpriteRect(Sprites::CURSOR, 0)).scaled(spriteSize)
				);
	}
}


