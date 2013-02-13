#pragma once
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>

class Sokoban {
public:
	enum CellType {
		FLOOR           = ' ',
		WALL            = '#',
		PLAYER_ON_FLOOR = '@',
		EMPTY_SLOT      = '.',
		PLAYER_ON_SLOT  = '+',
		BOX_ON_FLOOR    = '$',
		BOX_ON_SLOT     = '*'
	};
	typedef int Cell;
	enum Control {
		NONE       = 0,
		UP         = 'u',
		DOWN       = 'd',
		LEFT       = 'l',
		RIGHT      = 'r',
		PUSH_UP    = 'U',
		PUSH_DOWN  = 'D',
		PUSH_LEFT  = 'L',
		PUSH_RIGHT = 'R'
	};

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
	void processControls(const QString & controls);
	QString toString() const;
	QString historyAsString() const;
	bool undo();
	bool isSolved();
	Cell getCell(const QPoint & point) const { return cell(point); }
private:
	QSize size;
	QVector<Cell> cells;
	QString history;
	QPoint getPlayerPos() const;
	Cell & cell(const QPoint & point);
	const Cell & cell(const QPoint & point) const;
	bool isValid(const QPoint & pos) const;
};
