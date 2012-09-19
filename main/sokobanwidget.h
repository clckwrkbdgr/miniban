#pragma once
#include <QtGui/QWidget>
#include "levelset.h"

class AbstractGameMode;

class SokobanWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(SokobanWidget);
public:
	SokobanWidget(QWidget * parent = 0);
	virtual ~SokobanWidget();
signals:
	void wantsToQuit();
protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void resizeEvent(QResizeEvent*);
private slots:
	void loadNextLevel();
	void showMessage();
	void startFadeIn();
	void startGame();
private:
	QImage snapshot;
	LevelSet levelSet;
	AbstractGameMode * gameMode;
};

