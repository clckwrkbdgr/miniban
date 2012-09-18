#pragma once
#include <QtCore/QMap>
#include <QtGui/QWidget>
#include "levelset.h"

class GameMode : public QObject {
	Q_OBJECT
public:
	GameMode(QObject * parent = 0) : QObject(parent) {}
	virtual ~GameMode() {}

	virtual void invalidateRect() = 0;
	virtual void paint(QPainter * painter, const QRect & rect) = 0;
	virtual void processControl(int control) = 0;
};

class PlayingMode : public GameMode {
	Q_OBJECT
public:
	QString currentLevel;

	PlayingMode(const QString & level, QObject * parent = 0);
	virtual ~PlayingMode() {}
	
	virtual void invalidateRect();
	virtual void paint(QPainter * painter, const QRect & rect);
	virtual void processControl(int control);
signals:
	void levelIsSolved();
private:
	QString originalLevel;
	QSize currentLevelSize;
	QString history;
	QMap<QChar, QImage> sprites;
	QSize spriteSize;
	bool toInvalidate;

	void resizeSpritesForLevel(const QSize & levelSize, const QRect & rect);
	void restartLevel(const QString & level);
};

class MessageMode : public GameMode {
	Q_OBJECT
public:
	MessageMode(const QString & message, QObject * parent = 0);
	virtual ~MessageMode() {}

	virtual void invalidateRect();
	virtual void paint(QPainter * painter, const QRect & rect);
	virtual void processControl(int control);
private:
	QString messageToShow;

};

class SokobanWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(SokobanWidget);
public:
	SokobanWidget(QWidget * parent = 0);
	virtual ~SokobanWidget() {}
signals:
	void wantsToQuit();
protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void resizeEvent(QResizeEvent*);
private slots:
	void loadNextLevel();
private:
	LevelSet levelSet;
	GameMode * gameMode;
	/*
	int mode;
	GameMode * playingMode;
	GameMode * messageMode;
	*/

	void processControl(int control);
};

