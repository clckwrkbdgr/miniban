#include "playingmode.h"
#include "sokobanwidget.h"
#include "message.h"
#include <chthon2/log.h>
#include <chthon2/format.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

namespace {

std::map<int, std::string> generateKeyToTextMap()
{
	std::map<int, std::string> result;
	result[SDLK_0]         = "0";
	result[SDLK_1]         = "1";
	result[SDLK_BACKSPACE] = "Backspace";
	result[SDLK_DOWN]      = "Down";
	result[SDLK_h]         = "H";
	result[SDLK_HOME]      = "Home";
	result[SDLK_j]         = "J";
	result[SDLK_k]         = "K";
	result[SDLK_l]         = "L";
	result[SDLK_y]         = "Y";
	result[SDLK_u]         = "U";
	result[SDLK_b]         = "B";
	result[SDLK_n]         = "N";
	result[SDLK_LEFT]      = "Left";
	result[SDLK_o]         = "O";
	result[SDLK_q]         = "Q";
	result[SDLK_r]         = "R";
	result[SDLK_RIGHT]     = "Right";
	result[SDLK_SPACE]     = "Space";
	result[SDLK_u]         = "U";
	result[SDLK_UP]        = "Up";
	result[SDLK_z]         = "Z";
	result[SDLK_x]         = "X";
	result[SDLK_PERIOD]    = ".";
	result[SDLK_ESCAPE]    = "Esc";
	return result;
}

std::map<std::string, int> generateTextToControlMap()
{
	std::map<std::string, int> result;
	result["Left"]  = result["H"] = Game::CONTROL_LEFT;
	result["Down"]  = result["J"] = Game::CONTROL_DOWN;
	result["Up"]    = result["K"] = Game::CONTROL_UP;
	result["Right"] = result["L"] = Game::CONTROL_RIGHT;
	result["Y"] = Game::CONTROL_UP_LEFT;
	result["U"] = Game::CONTROL_UP_RIGHT;
	result["B"] = Game::CONTROL_DOWN_LEFT;
	result["N"] = Game::CONTROL_DOWN_RIGHT;
	result["Shift-Left"]  = result["Shift-H"] = Game::CONTROL_RUN_LEFT;
	result["Shift-Down"]  = result["Shift-J"] = Game::CONTROL_RUN_DOWN;
	result["Shift-Up"]    = result["Shift-K"] = Game::CONTROL_RUN_UP;
	result["Shift-Right"] = result["Shift-L"] = Game::CONTROL_RUN_RIGHT;
	result["X"]         = Game::CONTROL_TARGET;
	result["."]         = Game::CONTROL_GOTO;
	result["Space"]     = Game::CONTROL_SKIP;
	result["Ctrl-Z"]    = Game::CONTROL_UNDO;
	result["Backspace"] = Game::CONTROL_UNDO;
	result["Ctrl-R"]    = Game::CONTROL_HOME;
	result["Home"]      = Game::CONTROL_HOME;
	result["Ctrl-Q"]    = Game::CONTROL_QUIT;
	result["Q"]         = Game::CONTROL_QUIT;
	result["Esc"]       = Game::CONTROL_ESCAPE;

	result["Ctrl-0"]    = Game::CONTROL_CHEAT_RESTART;
	result["Ctrl-1"]    = Game::CONTROL_CHEAT_SKIP_LEVEL;
	return result;
}

std::map<int, std::string> keyToText = generateKeyToTextMap();
std::map<std::string, int> textToControl = generateTextToControlMap();

}


SokobanWidget::SokobanWidget(int argc, char ** argv)
	: quit(false)
{
	char absolute_file_path[256] = {0};
	const char * ok = realpath(argv[1], absolute_file_path);
	std::string commandLineFilename = (argc <= 1 && ok) ? "" : absolute_file_path;

	settings.load();
	if(settings.levelset.empty()) {
		if(commandLineFilename.empty()) {
			levelSet = LevelSet();
		} else {
			levelSet.loadFromFile(commandLineFilename, 0);
		}
	} else {
		if(commandLineFilename.empty()) {
			levelSet.loadFromFile(settings.levelset, settings.level_index);
		} else {
			if(commandLineFilename == settings.levelset) {
				levelSet.loadFromFile(settings.levelset, settings.level_index);
			} else {
				levelSet.loadFromFile(commandLineFilename, 0);
			}
		}
	}
}

SokobanWidget::~SokobanWidget()
{
	settings.level_index = levelSet.getCurrentLevelIndex();
	settings.levelset = levelSet.getCurrentLevelSet();
	settings.save();
}

int SokobanWidget::keyToControl(SDL_KeyboardEvent * event)
{
	bool isShiftDown = event->keysym.mod & (KMOD_RSHIFT | KMOD_LSHIFT);
	bool isCtrlDown = event->keysym.mod & (KMOD_RCTRL | KMOD_LCTRL);

	std::string pressedCombination = keyToText[event->keysym.sym];
	if(isShiftDown) {
		pressedCombination = "Shift-" + pressedCombination;
	}
	if(isCtrlDown) {
		pressedCombination = "Ctrl-" + pressedCombination;
	}

	return textToControl[pressedCombination];
}

int SokobanWidget::exec()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * window = SDL_CreateWindow(
			"Sokoban",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			0, 0,
			SDL_WINDOW_FULLSCREEN_DESKTOP
			);
	SDL_ShowCursor(0);
	rect.x = 0;
	rect.y = 0;
	SDL_GetWindowSize(window, &rect.w, &rect.h);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	sprites.init(renderer);

	settings.level_index = levelSet.getCurrentLevelIndex();
	settings.levelset = levelSet.getCurrentLevelSet();
	settings.save();

	Game game = Game(levelSet.getCurrentSokoban(), sprites);
	Message message = Message(sprites,
			levelSet.isOver()
			? Chthon::format("{0}\nLevels are over.", levelSet.getLevelSetTitle())
			: Chthon::format(
				"{0}: {3}\n{1}/{2}",
				levelSet.getLevelSetTitle(),
				levelSet.getCurrentLevelIndex() + 1,
				levelSet.getLevelCount(),
				levelSet.getCurrentLevelName(),
				"DUMMY"
				)
			);
	bool show_message = true;

	SDL_Event event;
	Uint32 last_time = SDL_GetTicks();
	while(!quit) {
		if(show_message) {
			message.paint(renderer, rect);
		} else {
			game.paint(renderer, rect);
		}

		SDL_RenderPresent(renderer);

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				int control = keyToControl(&event.key);
				if(control == Game::CONTROL_QUIT) {
					quit = true;
				}
				if(show_message) {
					message.processControl(control);
				} else {
					game.processControl(control);
				}
			} else if(event.type == SDL_QUIT) {
				quit = true;
			}
		}

		Uint32 current_time = SDL_GetTicks();
		int time_passed = current_time - last_time;
		if(time_passed > 0) {
			last_time = current_time;
			if(show_message) {
				message.processTime(time_passed);
			} else {
				game.processTime(time_passed);
			}
		}

		if(show_message) {
			if(message.is_done() && !levelSet.isOver()) {
				if(!levelSet.isOver()) {
					show_message = false;
				}
			}
		} else {
			if(game.is_done()) {
				settings.level_index = levelSet.getCurrentLevelIndex();
				settings.levelset = levelSet.getCurrentLevelSet();
				settings.save();

				levelSet.moveToNextLevel();
				if(!levelSet.isOver()) {
					game.load(levelSet.getCurrentSokoban());
				}

				message.set_text(
						levelSet.isOver()
						? Chthon::format("{0}\nLevels are over.", levelSet.getLevelSetTitle())
						: Chthon::format(
							"{0}: {3}\n{1}/{2}",
							levelSet.getLevelSetTitle(),
							levelSet.getCurrentLevelIndex() + 1,
							levelSet.getLevelCount(),
							levelSet.getCurrentLevelName(),
							"DUMMY"
							)
						);
				show_message = true;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
