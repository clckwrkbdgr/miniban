#include <QtDebug>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include "fademode.h"

namespace { // Aux.

const int FADE_DELAY = 1; // msec
const int FADE_DELTA = 10;
const int FADE_COUNT = 200;

};

FadeMode::FadeMode(const QImage & snapshotToFade, bool fadeOut, QObject * parent)
	: AbstractGameMode(parent), snapshot(snapshotToFade), isFadeOut(fadeOut), currentFade(isFadeOut ? FADE_COUNT : 0)
{
	timerId = startTimer(FADE_DELAY);
}

FadeMode::~FadeMode()
{
	if(timerId) {
		killTimer(timerId);
	}
}

void FadeMode::timerEvent(QTimerEvent*)
{
	bool isFadeEnded = (isFadeOut && currentFade <= 0) || (!isFadeOut && currentFade >= FADE_COUNT);
	if(isFadeEnded) {
		emit fadeIsEnded();
		killTimer(timerId);
		timerId = 0;
	}
	currentFade += isFadeOut ? -FADE_DELTA : FADE_DELTA;
	emit update();
}

void FadeMode::paint(QPainter * painter, const QRect & rect)
{
	painter->fillRect(rect, Qt::black);
	painter->setOpacity(double(currentFade) / double(FADE_COUNT));
	QPoint offset = QPoint(rect.width() - snapshot.width(), rect.height() - snapshot.height()) / 2;
	painter->drawImage(offset, snapshot);
}
