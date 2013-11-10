#pragma once
#include <QtGui/QWidget>
#include "levelset.h"
#include "sprites.h"

class AbstractGameMode;

class SokobanWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(SokobanWidget);
public:
	SokobanWidget(QWidget * parent = 0);
	virtual ~SokobanWidget();
protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void resizeEvent(QResizeEvent*);
private slots:
	void loadNextLevel();
	void showInterlevelMessage();
	void startFadeIn();
	void startGame();
private:
	QImage snapshot;
	LevelSet levelSet;
	AbstractGameMode * gameMode;
	Sprites sprites;
};

