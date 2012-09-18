#pragma once
#include <QtCore/QObject>

class QPainter;
class QRect;

class AbstractGameMode : public QObject {
	Q_OBJECT
public:
	enum { CONTROL_NONE, CONTROL_CHEAT, CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN, CONTROL_UNDO, CONTROL_HOME } Control;

	AbstractGameMode(QObject * parent = 0) : QObject(parent) {}
	virtual ~AbstractGameMode() {}

	virtual void invalidateRect() = 0;
	virtual void paint(QPainter * painter, const QRect & rect) = 0;
	virtual void processControl(int control) = 0;
};

