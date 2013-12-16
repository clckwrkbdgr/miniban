#pragma once
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QPoint>
#include <QtCore/QSize>

struct Cell {
	enum { SPACE, FLOOR, WALL, SLOT };
	int type;
	int sprite;
	explicit Cell(int cell_type = SPACE) : type(cell_type), sprite(0) {}
};

struct Object {
	QPoint pos;
	bool is_player;
	int sprite;
	Object() : is_player(false) {}
	explicit Object(const QPoint & object_pos, bool player = false) : pos(object_pos), is_player(player), sprite(0) {}
	bool isNull() const { return pos.isNull(); }
};

class Sokoban {
public:
	enum { LEFT, RIGHT, DOWN, UP, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };

	class InvalidPlayerCountException {
	public:
		InvalidPlayerCountException(int wrongPlayerCount) : playerCount(wrongPlayerCount) {}
		int playerCount;
	};
	class InvalidUndoException {
	public:
		InvalidUndoException(const QChar & control) : invalidUndoControl(control) {}
		QChar invalidUndoControl;
	};
	class OutOfMapException {};

	Sokoban();
	Sokoban(const QString & levelField, const QString & backgroundHistory = QString(), bool isFullHistoryTracked = false);
	virtual ~Sokoban() {}

	bool isValid() const { return valid; }
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
	void restart();
private:
	bool valid;
	QString original_level;
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
