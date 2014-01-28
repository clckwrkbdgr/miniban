#include <QtDebug>
#include "sokoban.h"
#include "sprites.h"
#include "playingmode.h"
#include <SDL2/SDL.h>

const int MIN_SCALE_FACTOR = 1;
const int MAX_SCALE_FACTOR = 8;

Game::Game(const Sokoban & prepared_sokoban, const Sprites & _sprites)
	: original_sprites(_sprites), toInvalidate(true),
	sokoban(prepared_sokoban), target_mode(false)
{
}

void Game::resizeSpritesForLevel(const QRect & rect)
{
	QSize originalSize = original_sprites.getSpritesBounds();
	int scaleFactor = qMin(
			rect.width() / (sokoban.width() * originalSize.width()),
			rect.height() / (sokoban.height() * originalSize.height())
			);
	scaleFactor = qBound(MIN_SCALE_FACTOR, scaleFactor, MAX_SCALE_FACTOR);

	spriteSize = originalSize * scaleFactor;
}

void Game::processControl(int control)
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
				err << QObject::tr("Invalid undo: %1. History string: '%2'").arg(e.invalidUndoControl).arg(QString::fromStdString(sokoban.historyAsString())) << endl;
			}
			break;
		default: return;
	}
	/*
	if(sokoban.isSolved()) {
		emit levelIsSolved();
	}
	*/
}

/*
void PlayingMode::invalidateRect()
{
	toInvalidate = true;
}
*/

bool Game::is_done() const
{
	return sokoban.isSolved();
}

//void PlayingMode::paint(SDL_Renderer * painter, const QRect & rect)
void Game::paint(SDL_Renderer * painter, const QRect & rect)
{
	if(toInvalidate) {
		resizeSpritesForLevel(rect);
		toInvalidate = false;
	}

	SDL_RenderClear(painter);

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

			QRect src_qrect = original_sprites.getSpriteRect(cellSprite, cell.sprite);
			SDL_Rect src_rect;
			src_rect.x = src_qrect.x();
			src_rect.y = src_qrect.y();
			src_rect.w = src_qrect.width();
			src_rect.h = src_qrect.height();
			SDL_Rect dest_rect;
			dest_rect.x = pos.x();
			dest_rect.y = pos.y();
			dest_rect.w = spriteSize.width();
			dest_rect.h = spriteSize.height();
			SDL_RenderCopy(painter, original_sprites.getTileSet(), &src_rect, &dest_rect);

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
					QRect src_qrect = original_sprites.getSpriteRect(objectSprite, object.sprite);
					SDL_Rect src_rect;
					src_rect.x = src_qrect.x();
					src_rect.y = src_qrect.y();
					src_rect.w = src_qrect.width();
					src_rect.h = src_qrect.height();
					SDL_RenderCopy(painter, original_sprites.getTileSet(), &src_rect, &dest_rect);
				}
			}
		}
	}
	if(target_mode) {
		QPoint pos = offset + QPoint(target.x() * spriteSize.width(), target.y() * spriteSize.height());
		QRect src_qrect = original_sprites.getSpriteRect(Sprites::CURSOR, 0);
		SDL_Rect src_rect;
		src_rect.x = src_qrect.x();
		src_rect.y = src_qrect.y();
		src_rect.w = src_qrect.width();
		src_rect.h = src_qrect.height();
		SDL_Rect dest_rect;
		dest_rect.x = pos.x();
		dest_rect.y = pos.y();
		dest_rect.w = spriteSize.width();
		dest_rect.h = spriteSize.height();
		SDL_RenderCopy(painter, original_sprites.getTileSet(), &src_rect, &dest_rect);
	}
}


