#include "sokoban.h"
#include "sprites.h"
#include "playingmode.h"
#include <chthon2/format.h>
#include <SDL2/SDL.h>
#include <iostream>

const int MIN_SCALE_FACTOR = 1;
const int MAX_SCALE_FACTOR = 8;

Game::Game(const Sokoban & prepared_sokoban, const Sprites & _sprites)
	: original_sprites(_sprites), toInvalidate(true),
	sokoban(prepared_sokoban), target_mode(false),
	fader_in(640), fader_out(640)
{
	fader_in.start();
}

void Game::load(const Sokoban & prepared_sokoban)
{
	fader_in.start();
	sokoban = prepared_sokoban;
	target_mode = false;
	toInvalidate = true;
}

void Game::resizeSpritesForLevel(const SDL_Rect & rect)
{
	SDL_Rect originalSize = original_sprites.getSpritesBounds();
	int scaleFactor = std::min(
			rect.w / (sokoban.width() * originalSize.w),
			rect.h / (sokoban.height() * originalSize.h)
			);
	scaleFactor = std::max(MIN_SCALE_FACTOR, std::min(scaleFactor, MAX_SCALE_FACTOR));

	sprite_width = originalSize.w * scaleFactor;
	sprite_height = originalSize.h * scaleFactor;
}

void Game::processControl(int control)
{
	if(sokoban.isSolved()) {
		return;
	}
	if(target_mode) {
		Chthon::Point new_target = target;
		switch(control) {
			case CONTROL_LEFT:  new_target.x--; break;
			case CONTROL_DOWN:  new_target.y++; break;
			case CONTROL_UP:    new_target.y--; break;
			case CONTROL_RIGHT: new_target.x++; break;
			case CONTROL_UP_LEFT: new_target += Chthon::Point(-1, -1); break;
			case CONTROL_UP_RIGHT: new_target += Chthon::Point(1, -1); break;
			case CONTROL_DOWN_LEFT: new_target += Chthon::Point(-1, 1); break;
			case CONTROL_DOWN_RIGHT: new_target += Chthon::Point(1, 1); break;
			case CONTROL_GOTO:
				sokoban.movePlayer(Chthon::Point(target.x, target.y));
				target_mode = false;
				break;
			case CONTROL_TARGET:  target_mode = false; break;
			default: break;
		}
		if(sokoban.isValid(Chthon::Point(new_target.x, new_target.y))) {
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
			target = Chthon::Point(player.x, player.y);
			break;
		}
		case CONTROL_HOME: sokoban.restart(); break;
		case CONTROL_UNDO:
			try {
				sokoban.undo();
			} catch(const Sokoban::InvalidUndoException & e) {
				std::cerr << Chthon::format("Invalid undo: {0}. History string: '{1}'", e.invalidUndoControl, sokoban.historyAsString()) << std::endl;
			}
			break;
		default: return;
	}
	if(sokoban.isSolved()) {
		fader_out.start();
	}
}

bool Game::is_done() const
{
	return sokoban.isSolved() && !fader_out.is_active();
}

void Game::paint(SDL_Renderer * painter, const SDL_Rect & rect)
{
	if(toInvalidate) {
		resizeSpritesForLevel(rect);
		toInvalidate = false;
	}

	SDL_RenderClear(painter);

	Chthon::Point offset = Chthon::Point(
			rect.w - sokoban.width() * sprite_width,
			rect.h - sokoban.height() * sprite_height
			) / 2;
	for(int y = 0; y < sokoban.height(); ++y) {
		for(int x = 0; x < sokoban.width(); ++x) {
			Chthon::Point pos = offset + Chthon::Point(x * sprite_width, y * sprite_height);

			Cell cell = sokoban.getCellAt(x, y);
			int cellSprite = Sprites::SPACE;
			switch(cell.type) {
				case Cell::FLOOR: cellSprite = Sprites::FLOOR; break;
				case Cell::SLOT: cellSprite = Sprites::EMPTY_SLOT; break;
				case Cell::SPACE: cellSprite = Sprites::SPACE; break;
				case Cell::WALL: cellSprite = Sprites::WALL; break;
			}
			cellSprite = original_sprites.contains(cellSprite) ? cellSprite : Sprites::SPACE;

			SDL_Rect src_rect = original_sprites.getSpriteRect(cellSprite, cell.sprite);
			SDL_Rect dest_rect;
			dest_rect.x = pos.x;
			dest_rect.y = pos.y;
			dest_rect.w = sprite_width;
			dest_rect.h = sprite_height;
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
					SDL_Rect src_rect = original_sprites.getSpriteRect(objectSprite, object.sprite);
					SDL_RenderCopy(painter, original_sprites.getTileSet(), &src_rect, &dest_rect);
				}
			}
		}
	}
	if(target_mode) {
		Chthon::Point pos = offset + Chthon::Point(target.x * sprite_width, target.y * sprite_height);
		SDL_Rect src_rect = original_sprites.getSpriteRect(Sprites::CURSOR, 0);
		SDL_Rect dest_rect;
		dest_rect.x = pos.x;
		dest_rect.y = pos.y;
		dest_rect.w = sprite_width;
		dest_rect.h = sprite_height;
		SDL_RenderCopy(painter, original_sprites.getTileSet(), &src_rect, &dest_rect);
	}

	if(fader_in.is_active() || fader_out.is_active()) {
		if(fader_in.is_active()) {
			SDL_SetRenderDrawColor(painter, 0, 0, 0, 255 - fader_in.multiplied_value(255));
		} else {
			SDL_SetRenderDrawColor(painter, 0, 0, 0, fader_out.multiplied_value(255));
		}
		SDL_RenderFillRect(painter, &rect);
	}
}

void Game::processTime(int msec_passed)
{
	fader_in.tick(msec_passed);
	fader_out.tick(msec_passed);
}

