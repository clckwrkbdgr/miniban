#include "messagemode.h"
#include <SDL2/SDL.h>

namespace { // Aux.

const int MAX_CHAR_COUNT_IN_MESSAGE = 60;
const int MESSAGE_DELAY = 1000;

}

MessageMode::MessageMode(bool passable, const QString & message, QObject * parent)
	: AbstractGameMode(parent), timerId(0), isPassable(passable), messageToShow(message)
{
	if(isPassable) {
		timerId = startTimer(MESSAGE_DELAY);
	}
}

MessageMode::~MessageMode()
{
	if(timerId) {
		killTimer(timerId);
	}
}

void MessageMode::invalidateRect()
{
}

void MessageMode::timerEvent(QTimerEvent*)
{
	if(isPassable) {
		emit messageIsEnded();
		killTimer(timerId);
		timerId = 0;
	}
}

void MessageMode::processControl(int control)
{
	if(isPassable && control == CONTROL_SKIP) {
		emit messageIsEnded();
		killTimer(timerId);
		timerId = 0;
	}
}

void MessageMode::paint(SDL_Renderer * painter, const QRect & /*rect*/)
{
	SDL_RenderClear(painter);

	// TODO draw text using font.
	/* TODO painter->setPen(Qt::white);
	QFont f = painter->font();
	f.setPixelSize(rect.width() / MAX_CHAR_COUNT_IN_MESSAGE);
	painter->setFont(f);
	painter->drawText(rect, Qt::AlignCenter, messageToShow);
	*/
}

