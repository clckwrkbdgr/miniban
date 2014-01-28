#include <QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include "playingmode.h"
#include "messagemode.h"
#include "fademode.h"
#include "sokobanwidget.h"
#include <chthon/log.h>
#include <chthon/format.h>
#include <SDL2/SDL.h>
#include <algorithm>
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
	Message(const Sprites & _sprites, const std::string & message_text);
	void set_text(const std::string & message_text);
	bool is_done() const;
	void processControl(int control);
	void paint(SDL_Renderer * painter, const QRect & rect);
	void processTime(int msec_passed);
private:
	const Sprites & sprites;
	bool done;
	std::vector<std::string> lines;
	unsigned max_width;
	int msecs_to_done;
};

void Message::processTime(int msec_passed)
{
	if(!done && msecs_to_done > 0) {
		msecs_to_done -= msec_passed;
		if(msecs_to_done <= 0) {
			done = true;
		}
	}
}

Message::Message(const Sprites & _sprites, const std::string & message_text)
	: sprites(_sprites), done(false)
{
	set_text(message_text);
}

void Message::set_text(const std::string & message_text)
{
	msecs_to_done = 1000;
	done = false;
	lines.clear();
	lines.push_back(std::string());
	max_width = 0;
	for(char ch : message_text) {
		if(ch != '\n') {
			lines.back() += ch;
		} else {
			max_width = std::max(max_width, lines.back().size());
			lines.push_back(std::string());
		}
	}
	max_width = std::max(max_width, lines.back().size());
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

void Message::paint(SDL_Renderer * painter, const QRect & rect)
{
	SDL_RenderClear(painter);
	SDL_Rect dest_rect;
	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.w = sprites.getCharRect(0).width();
	dest_rect.h = sprites.getCharRect(0).height();

	QRect text_rect = QRect(0, 0, dest_rect.w * max_width, dest_rect.h * lines.size());
	text_rect.moveCenter(rect.center());
	dest_rect.y = text_rect.y();
	for(const std::string & line : lines) {
		dest_rect.x = text_rect.x() + (dest_rect.w * (max_width - line.size())) / 2 ;
		for(char ch : line) {
			QRect char_qrect = sprites.getCharRect(ch);
			SDL_Rect char_rect;
			char_rect.x = char_qrect.x();
			char_rect.y = char_qrect.y();
			char_rect.w = char_qrect.width();
			char_rect.h = char_qrect.height();

			SDL_RenderCopy(painter, sprites.getFont(), &char_rect, &dest_rect);
			dest_rect.x += dest_rect.w;
		}
		dest_rect.y += dest_rect.h;
	}
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
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	rect = QRect(0, 0, w, h);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	sprites.init(renderer);

	QSettings settings;
	settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());

	Game game = Game(levelSet.getCurrentSokoban(), sprites);
	Message message = Message(sprites,
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
				QSettings settings;
				settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());
				levelSet.moveToNextLevel();
				if(!levelSet.isOver()) {
					game.load(levelSet.getCurrentSokoban());
				}

				message.set_text(
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
				show_message = true;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
