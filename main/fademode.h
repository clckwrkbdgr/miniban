#pragma once
#include <QtGui/QImage>
#include "abstractgamemode.h"

class FadeMode : public AbstractGameMode {
	Q_OBJECT
public:
	FadeMode(const QImage & snapshotToFade, bool fadeOut, QObject * parent = 0);
	virtual ~FadeMode();

	virtual void invalidateRect() {}
	virtual void paint(QPainter * painter, const QRect & rect);
	virtual void processControl(int) {}
signals:
	void fadeIsEnded();
	void update();
protected:
	void timerEvent(QTimerEvent*);
private:
	QImage snapshot;
	bool isFadeOut;
	int timerId;
	int currentFade;
};

