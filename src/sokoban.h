#pragma once
#include <chthon2/map.h>
#include <chthon2/point.h>

struct Cell {
	enum { SPACE, FLOOR, WALL, SLOT };
	int type;
	int sprite;
	explicit Cell(int cell_type = SPACE) : type(cell_type), sprite(0) {}
};

struct Object {
	Chthon::Point pos;
	bool is_player;
	int sprite;
	Object() : is_player(false) {}
	explicit Object(const Chthon::Point & object_pos, bool player = false) : pos(object_pos), is_player(player), sprite(0) {}
	bool isNull() const { return pos.null(); }
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
		InvalidUndoException(const char & control) : invalidUndoControl(control) {}
		char invalidUndoControl;
	};
	class OutOfMapException {};

	Sokoban();
	Sokoban(const std::string & levelField, const std::string & backgroundHistory = std::string(), bool isFullHistoryTracked = false);
	virtual ~Sokoban() {}

	void load(const std::string & levelField, const std::string & backgroundHistory = std::string(), bool isFullHistoryTracked = false);

	bool isValid() const { return valid; }
	int width() const { return cells.width(); }
	int height() const { return cells.height(); }
	bool isValid(const Chthon::Point & pos) const;
	Cell getCellAt(int x, int y) const;
	Cell getCellAt(const Chthon::Point & point) const { return getCellAt(point.x, point.y); }
	Object getObjectAt(int x, int y) const;
	Object getObjectAt(const Chthon::Point & point) const { return getObjectAt(point.x, point.y); }

	std::string toString() const;
	std::string historyAsString() const;
	Chthon::Point getPlayerPos() const;

	bool undo();
	bool isSolved() const;
	bool movePlayer(int control, bool cautious = false);
	bool movePlayer(const Chthon::Point & target);
	bool runPlayer(int control);
	void restart();
private:
	bool valid;
	std::string original_level;
	Object player;
	std::vector<Object> boxes;
	Chthon::Map<Cell> cells;
	std::string history;
	bool has_box(const Chthon::Point & point) const;
	bool fullHistoryTracking;
	void fillFloor(std::vector<int> & reachable, const Chthon::Point & point);
	bool shiftPlayer(const Chthon::Point & shift);
};
