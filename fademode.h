#pragma once
#include <QtGui/QImage>
#include "abstractgamemode.h"
#include "sprites.h"

class FadeMode : public AbstractGameMode {
	Q_OBJECT
public:
	FadeMode(const QString & levelToFade, const Sprites & _sprites, bool fadeOut, QObject * parent = 0);
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
	QString level;
	QImage snapshot;
	Sprites sprites;
	bool isFadeOut;
	int timerId;
	int currentFade;
};

