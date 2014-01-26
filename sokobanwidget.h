#pragma once
#include "levelset.h"
#include "sprites.h"
class SDL_KeyboardEvent;
class SDL_Renderer;
class AbstractGameMode;

class SokobanWidget : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(SokobanWidget);
public:
	SokobanWidget(QObject * parent = 0);
	virtual ~SokobanWidget();
	int exec();
protected:
	void update();
	void keyPressEvent(SDL_KeyboardEvent * event);
private slots:
	void loadNextLevel();
	void showInterlevelMessage();
	void startFadeIn();
	void startGame();
private:
	SDL_Renderer * renderer;
	SDL_Texture * snapshot;
	LevelSet levelSet;
	AbstractGameMode * gameMode;
	Sprites sprites;
	bool quit;
	QRect rect;
};

