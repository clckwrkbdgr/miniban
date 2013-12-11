#pragma once
#include "abstractgamemode.h"
#include "sprites.h"
#include "sokoban.h"
#include <QtGui/QImage>

class FadeMode : public AbstractGameMode {
	Q_OBJECT
public:
	FadeMode(const Sokoban & levelToFade, const Sprites & _sprites, bool fadeOut, QObject * parent = 0);
	virtual ~FadeMode();

	virtual void invalidateRect();
	virtual void paint(QPainter * painter, const QRect & rect);
	virtual void processControl(int) {}
signals:
	void fadeIsEnded();
	void update();
protected:
	void timerEvent(QTimerEvent*);
private:
	Sokoban level;
	QImage snapshot;
	Sprites sprites;
	bool isFadeOut;
	int timerId;
	int currentFade;
};

