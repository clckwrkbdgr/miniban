#pragma once
#include <QtCore/QMap>
#include <QtGui/QWidget>
#include "levelset.h"

struct PlayingMode {
	QString currentLevel;

	PlayingMode(const QString & level);
	
	void invalidateRect();
	void paint(QPainter * painter, const QRect & rect);
	void processControl(int control, const LevelSet & levelSet);
private:
	QSize currentLevelSize;
	QString history;
	QMap<QChar, QImage> sprites;
	QSize spriteSize;
	bool toInvalidate;

	void resizeSpritesForLevel(const QSize & levelSize, const QRect & rect);
	void restartLevel(const QString & level);
};

struct MessageMode {
	bool toInvalidate;
	QString messageToShow;

	MessageMode(const QString & message);

	void invalidateRect();
	void paint(QPainter * painter, const QRect & rect);
	void processControl(int control);
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
private:
	LevelSet levelSet;
	int mode;
	PlayingMode playingMode;
	MessageMode messageMode;

	void loadNextLevel();
	void processControl(int control);
};

