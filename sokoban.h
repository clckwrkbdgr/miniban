#pragma once
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>

class Sokoban {
public:
	enum { FLOOR, SPACE, WALL, PLAYER_ON_FLOOR, EMPTY_SLOT, PLAYER_ON_SLOT, BOX_ON_FLOOR, BOX_ON_SLOT };
	enum { LEFT, RIGHT, DOWN, UP };
	typedef int Cell;

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

	Sokoban(const QString & levelField, const QString & backgroundHistory = QString());
	virtual ~Sokoban() {}

	int width() const { return size.width(); }
	int height() const { return size.height(); }
	bool isValid(const QPoint & pos) const;
	Cell getCell(int x, int y) const { return cell(QPoint(x, y)); }
	Cell getCell(const QPoint & point) const { return cell(point); }

	QString toString() const;
	QString historyAsString() const;
	QPoint getPlayerPos() const;

	bool undo();
	bool isSolved();
	bool movePlayer(int control, bool cautious = false);
	bool movePlayer(const QPoint & target);
	bool runPlayer(int control);
private:
	QSize size;
	QVector<Cell> cells;
	QString history;
	Cell & cell(const QPoint & point);
	const Cell & cell(const QPoint & point) const;
	void fillFloor(QVector<int> & reachable, const QPoint & point);
	bool shiftPlayer(const QPoint & shift);
};
