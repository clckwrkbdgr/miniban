#pragma once
#include "abstractgamemode.h"

class MessageMode : public AbstractGameMode {
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


