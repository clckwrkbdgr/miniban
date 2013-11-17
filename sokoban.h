#pragma once
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QPoint>
#include <QtCore/QSize>

class Sokoban {
public:
	enum { NONE = 0, FLOOR, SPACE, WALL, PLAYER_ON_FLOOR, EMPTY_SLOT, PLAYER_ON_SLOT, BOX_ON_FLOOR, BOX_ON_SLOT };
	enum { LEFT, RIGHT, DOWN, UP };
	typedef int Cell;
	typedef int Sprite;

	class InvalidPlayerCountException {
	public:
		InvalidPlayerCountException(int wrongPlayerCount) : playerCount(wrongPlayerCount) {}
		int playerCount;
	};
	class InvalidControlException {
	public:
		InvalidControlException(const QChar & control) : invalidControl(control) {}
		QChar invalidControl;
	};
	class InvalidUndoException {
	public:
		InvalidUndoException(const QChar & control) : invalidUndoControl(control) {}
		QChar invalidUndoControl;
	};
	class OutOfMapException {};

	Sokoban(const QString & levelField, const QString & backgroundHistory = QString(), bool isFullHistoryTracked = false);
	virtual ~Sokoban() {}

	int width() const { return size.width(); }
	int height() const { return size.height(); }
	bool isValid(const QPoint & pos) const;
	Sprite getCellSprite(const QPoint & point) const;
	Sprite getCellSprite(int x, int y) const { return getCellSprite(QPoint(x, y)); }
	Sprite getObjectSprite(const QPoint & point) const;
	Sprite getObjectSprite(int x, int y) const { return getObjectSprite(QPoint(x, y)); }

	QString toString() const;
	QString historyAsString() const;
	QPoint getPlayerPos() const;

	bool undo();
	bool isSolved();
	bool movePlayer(int control, bool cautious = false);
	bool movePlayer(const QPoint & target);
	bool runPlayer(int control);
private:
	QPoint player;
	QSize size;
	QVector<Cell> cells;
	QString history;
	bool fullHistoryTracking;
	Cell & cell(const QPoint & point);
	const Cell & cell(const QPoint & point) const;
	void fillFloor(QVector<int> & reachable, const QPoint & point);
	bool shiftPlayer(const QPoint & shift);
};
