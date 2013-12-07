#pragma once
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QPoint>
#include <QtCore/QSize>

struct Cell {
	enum { SPACE, FLOOR, WALL, SLOT };
	int type;
	explicit Cell(int cell_type = SPACE) : type(cell_type) {}
};

struct Object {
	QPoint pos;
	bool is_player;
	Object() : is_player(false) {}
	explicit Object(const QPoint & object_pos, bool player = false) : pos(object_pos), is_player(player) {}
	bool isNull() const { return pos.isNull(); }
};

class Sokoban {
public:
	enum { LEFT, RIGHT, DOWN, UP };

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
	Cell getCellAt(int x, int y) const;
	Cell getCellAt(const QPoint & point) const { return getCellAt(point.x(), point.y()); }
	Object getObjectAt(int x, int y) const;
	Object getObjectAt(const QPoint & point) const { return getObjectAt(point.x(), point.y()); }

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
	Object player;
	QList<Object> boxes;
	QVector<Cell> cells;
	QString history;
	bool has_box(const QPoint & point) const;
	bool fullHistoryTracking;
	Cell & cell(const QPoint & point);
	const Cell & cell(const QPoint & point) const;
	void fillFloor(QVector<int> & reachable, const QPoint & point);
	bool shiftPlayer(const QPoint & shift);
};
