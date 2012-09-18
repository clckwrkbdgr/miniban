#include <QtGui/QPainter>
#include "messagemode.h"

namespace { // Aux.

const int MAX_CHAR_COUNT_IN_MESSAGE = 60;

}

MessageMode::MessageMode(const QString & message, QObject * parent)
	: AbstractGameMode(parent), messageToShow(message)
{
}

void MessageMode::invalidateRect()
{
}

void MessageMode::processControl(int)
{
}

void MessageMode::paint(QPainter * painter, const QRect & rect)
{
	painter->fillRect(rect, Qt::black);
	painter->setPen(Qt::white);
	QFont f = painter->font();
	f.setPixelSize(rect.width() / MAX_CHAR_COUNT_IN_MESSAGE);
	painter->setFont(f);
	painter->drawText(rect, Qt::AlignCenter, messageToShow);
}


