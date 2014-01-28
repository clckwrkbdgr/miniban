#include <QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include "playingmode.h"
#include "messagemode.h"
#include "fademode.h"
#include "sokobanwidget.h"
#include <chthon/format.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <iostream>
using namespace Chthon;

namespace {

QMap<int, QString> generateKeyToTextMap()
{
	QMap<int, QString> result;
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
	return result;
}

QMap<QString, int> generateTextToControlMap()
{
	QMap<QString, int> result;
	result["Left"]  = result["H"] = AbstractGameMode::CONTROL_LEFT;
	result["Down"]  = result["J"] = AbstractGameMode::CONTROL_DOWN;
	result["Up"]    = result["K"] = AbstractGameMode::CONTROL_UP;
	result["Right"] = result["L"] = AbstractGameMode::CONTROL_RIGHT;
	result["Y"] = AbstractGameMode::CONTROL_UP_LEFT;
	result["U"] = AbstractGameMode::CONTROL_UP_RIGHT;
	result["B"] = AbstractGameMode::CONTROL_DOWN_LEFT;
	result["N"] = AbstractGameMode::CONTROL_DOWN_RIGHT;
	result["Shift-Left"]  = result["Shift-H"] = AbstractGameMode::CONTROL_RUN_LEFT;
	result["Shift-Down"]  = result["Shift-J"] = AbstractGameMode::CONTROL_RUN_DOWN;
	result["Shift-Up"]    = result["Shift-K"] = AbstractGameMode::CONTROL_RUN_UP;
	result["Shift-Right"] = result["Shift-L"] = AbstractGameMode::CONTROL_RUN_RIGHT;
	result["X"]         = AbstractGameMode::CONTROL_TARGET;
	result["."]         = AbstractGameMode::CONTROL_GOTO;
	result["Space"]     = AbstractGameMode::CONTROL_SKIP;
	result["Ctrl-Z"]    = AbstractGameMode::CONTROL_UNDO;
	result["Backspace"] = AbstractGameMode::CONTROL_UNDO;
	result["Ctrl-R"]    = AbstractGameMode::CONTROL_HOME;
	result["Home"]      = AbstractGameMode::CONTROL_HOME;
	result["Ctrl-Q"]    = AbstractGameMode::CONTROL_QUIT;
	result["Q"]         = AbstractGameMode::CONTROL_QUIT;

	result["Ctrl-0"]    = AbstractGameMode::CONTROL_CHEAT_RESTART;
	result["Ctrl-1"]    = AbstractGameMode::CONTROL_CHEAT_SKIP_LEVEL;
	return result;
}

const QMap<int, QString> keyToText = generateKeyToTextMap();
const QMap<QString, int> textToControl = generateTextToControlMap();

}

SokobanWidget::SokobanWidget()
	: quit(false)
{
	QStringList args = QCoreApplication::arguments();
	args.removeAt(0);
	QString commandLineFilename = args.isEmpty() ? QString() : QFileInfo(args[0]).absoluteFilePath();

	QSettings settings;
	int lastLevelIndex = settings.value("levels/lastindex", 0).toInt();
	QString lastLevelSet = settings.value("levels/levelset", QString()).toString();
	if(lastLevelSet.isEmpty()) {
		if(commandLineFilename.isEmpty()) {
			levelSet = LevelSet();
		} else {
			levelSet = LevelSet(commandLineFilename, 0);
		}
	} else {
		if(commandLineFilename.isEmpty()) {
			levelSet = LevelSet(lastLevelSet, lastLevelIndex);
		} else {
			if(commandLineFilename == lastLevelSet) {
				levelSet = LevelSet(lastLevelSet, lastLevelIndex);
			} else {
				levelSet = LevelSet(commandLineFilename, 0);
			}
		}
	}

	//showInterlevelMessage();
}

SokobanWidget::~SokobanWidget()
{
	QSettings settings;
	settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());
	settings.setValue("levels/levelset", levelSet.getCurrentLevelSet());
}

int SokobanWidget::keyToControl(SDL_KeyboardEvent * event)
{
	bool isShiftDown = event->keysym.mod & (KMOD_RSHIFT | KMOD_LSHIFT);
	bool isCtrlDown = event->keysym.mod & (KMOD_RCTRL | KMOD_LCTRL);

	QString pressedCombination = keyToText[event->keysym.sym];
	if(isShiftDown) {
		pressedCombination.prepend("Shift-");
	}
	if(isCtrlDown) {
		pressedCombination.prepend("Ctrl-");
	}

	return textToControl.value(pressedCombination);
}

/*
void SokobanWidget::loadNextLevel()
{
	if(levelSet.isOver())
		return;
	Sokoban level = levelSet.getCurrentSokoban();
	PlayingMode * playingMode = static_cast<PlayingMode*>(gameMode);
	if(playingMode) {
		level = playingMode->getCurrentSokoban();
	}
	levelSet.moveToNextLevel();

	gameMode = new FadeMode(level, sprites, true, this);
	connect(gameMode, SIGNAL(fadeIsEnded()), this, SLOT(showInterlevelMessage()));
	connect(gameMode, SIGNAL(update()), this, SLOT(update()));
	update();
}

void SokobanWidget::showInterlevelMessage()
{
	QString levelName = tr("%1: %4\n%2/%3")
		.arg(levelSet.getLevelSetTitle())
		.arg(levelSet.getCurrentLevelIndex() + 1)
		.arg(levelSet.getLevelCount())
		.arg(levelSet.getCurrentLevelName())
		;
	QString message = levelSet.isOver() ? tr("%1\nLevels are over.").arg(levelSet.getLevelSetTitle()) : levelName;
	gameMode = new MessageMode(!levelSet.isOver(), message, this);
	connect(gameMode, SIGNAL(messageIsEnded()), this, SLOT(startFadeIn()));
	update();
}

void SokobanWidget::startFadeIn()
{
	gameMode = new FadeMode(levelSet.getCurrentSokoban(), sprites, false, this);
	connect(gameMode, SIGNAL(fadeIsEnded()), this, SLOT(startGame()));
	connect(gameMode, SIGNAL(update()), this, SLOT(update()));
	update();
}

void SokobanWidget::startGame()
{
	QSettings settings;
	settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());
	gameMode = new PlayingMode(levelSet.getCurrentSokoban(), sprites, this);
	//connect(gameMode, SIGNAL(levelIsSolved()), this, SLOT(loadNextLevel()));
	update();
}

void SokobanWidget::update()
{
	SDL_RenderClear(renderer);

}
*/

class Message {
public:
	Message(SDL_Renderer * renderer, TTF_Font * font, const std::string & message_text);
	bool is_done() const;
	void processControl(int control);
	void paint(SDL_Renderer * painter, const QRect & rect);
private:
	SDL_Texture * texture;
	std::string text;
	bool done;
};


Message::Message(SDL_Renderer * renderer, TTF_Font * font, const std::string & message_text)
	: text(message_text), done(false)
{
	SDL_Color text_color = {255, 0, 0, 255};
	SDL_Surface * text_surface = TTF_RenderText_Solid(font,
			message_text.c_str(),
			text_color);
	texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

bool Message::is_done() const
{
	return done;
}

void Message::processControl(int control)
{
	if(control == Game::CONTROL_SKIP) {
		done = true;
	}
}

void Message::paint(SDL_Renderer * painter, const QRect & /*rect*/)
{
	SDL_RenderClear(painter);
	SDL_RenderCopy(painter, texture, 0, 0);
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
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	rect = QRect(0, 0, w, h);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	sprites.init(renderer);

	TTF_Init();
	TTF_Font * font = TTF_OpenFont("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf", 24);
	if(font == NULL)
	{
		std::cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << std::endl;
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	QSettings settings;
	settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());

	Game game = Game(levelSet.getCurrentSokoban(), sprites);
	Message message = Message(renderer, font,
			levelSet.isOver()
			? format("{0}\nLevels are over.", levelSet.getLevelSetTitle().toStdString())
			: format(
				"{0}: {3}\n{1}/{2}",
				levelSet.getLevelSetTitle().toStdString(),
				levelSet.getCurrentLevelIndex() + 1,
				levelSet.getLevelCount(),
				levelSet.getCurrentLevelName().toStdString(),
				"DUMMY"
				)
			);
	bool show_message = true;

	SDL_Event event;
	while(!quit) {
		while(SDL_PollEvent(&event)) {
			if(show_message) {
				message.paint(renderer, rect);
			} else {
				game.paint(renderer, rect);
			}

			SDL_RenderPresent(renderer);

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

			if(show_message) {
				if(message.is_done() && !levelSet.isOver()) {
					if(!levelSet.isOver()) {
						QSettings settings;
						settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());
						game.load(levelSet.getCurrentSokoban());
						levelSet.moveToNextLevel();
						show_message = false;
					}
				}
			} else {
				if(game.is_done()) {
					show_message = true;
					message = Message(renderer, font,
							levelSet.isOver()
							? format("{0}\nLevels are over.", levelSet.getLevelSetTitle().toStdString())
							: format(
								"{0}: {3}\n{1}/{2}",
								levelSet.getLevelSetTitle().toStdString(),
								levelSet.getCurrentLevelIndex() + 1,
								levelSet.getLevelCount(),
								levelSet.getCurrentLevelName().toStdString(),
								"DUMMY"
								)
							);
				}
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
