#include "sokoban.h"
#include <chthon2/util.h>
#include <chthon2/log.h>
#include <algorithm>
#include <sstream>
#include <map>
#include <set>

static const bool DIRECTIONAL_PLAYER_SPRITES = false;

Sokoban::Sokoban()
	: valid(false), cells(1, 1)
{
}

Sokoban::Sokoban(const std::string & levelField, const std::string & backgroundHistory, bool isFullHistoryTracked)
	: Sokoban()
{
	load(levelField, backgroundHistory, isFullHistoryTracked);
}

void Sokoban::load(const std::string & levelField, const std::string & backgroundHistory, bool isFullHistoryTracked)
{
	valid = false;
	std::vector<std::string> rows = Chthon::split(levelField);
	unsigned h = rows.size();
	unsigned w = 0;
	for(const std::string & row : rows) {
		if(w < row.size()) {
			w = row.size();
		}
	}

	cells = Chthon::Map<Cell>(w, h);
	player = Object();
	unsigned playerCount = 0;
	for(unsigned y = 0; y < rows.size(); ++y) {
		const std::string & row = rows[y];
		for(unsigned x = 0; x < row.size(); ++x) {
			Chthon::Point pos(x, y);
			switch(row[x]) {
				case ' ': cells.cell(pos).type = Cell::SPACE; break;
				case '#': cells.cell(pos).type = Cell::WALL; break;
				case '@': cells.cell(pos).type = Cell::SPACE; player = Object(pos, true); ++playerCount; break;
				case '.': cells.cell(pos).type = Cell::SLOT; break;
				case '+': cells.cell(pos).type = Cell::SLOT; player = Object(pos, true); ++playerCount; break;
				case '$': cells.cell(pos).type = Cell::SPACE; boxes << Object(pos); break;
				case '*': cells.cell(pos).type = Cell::SLOT; boxes << Object(pos); break;
			}
			int sprite_chance = rand() % 100;
			if(sprite_chance < 50) {
				cells.cell(pos).sprite = 0;
			} else if(sprite_chance < 80) {
				cells.cell(pos).sprite = 1;
			} else if(sprite_chance < 95) {
				cells.cell(pos).sprite = 2;
			} else {
				cells.cell(pos).sprite = 3;
			}
		}
	}
	if(playerCount != 1) {
		throw InvalidPlayerCountException(playerCount);
	}
	for(unsigned i = 0; i < boxes.size(); ++i) {
		boxes[i].sprite = rand() % 4;
	}

	// 0 - passable, 1 - impassable, 2 - found to be floor.
	std::vector<int> reachable(cells.width() * cells.height(), 0);
	for(unsigned i = 0; i < reachable.size(); ++i) {
		reachable[i] = (cells.cell(i % width(), i / width()).type == Cell::WALL) ? 1 : 0;
	}
	fillFloor(reachable, getPlayerPos());
	for(unsigned i = 0; i < reachable.size(); ++i) {
		if(reachable[i] == 2 && cells.cell(i % width(), i / width()).type == Cell::SPACE) {
			cells.cell(i % width(), i / width()).type = Cell::FLOOR;
		}
	}
	valid = true;
	history = backgroundHistory;
	fullHistoryTracking = isFullHistoryTracked;
}

void Sokoban::restart()
{
	while(undo()) {
	}
}

bool Sokoban::has_box(const Chthon::Point & point) const
{
	foreach(const Object & box, boxes) {
		if(box.pos == point) {
			return true;
		}
	}
	return false;
}

bool Sokoban::movePlayer(const Chthon::Point & target)
{
	if(!valid) {
		return false;
	}
	std::vector<int> passed(cells.width() * cells.height(), -1);
	for(unsigned i = 0; i < passed.size(); ++i) {
		Chthon::Point p(i % width(), i / width());
		if(cells.cell(p).type != Cell::WALL && !has_box(p)) {
			passed[i] = 0;
		}
	}
	Chthon::Point pos = getPlayerPos();
	if(pos == target) {
		return true;
	}
	passed[pos.x + pos.y * width()] = 1;
	std::vector<Chthon::Point> current_points;
	current_points << pos;
	int counter = cells.width() * cells.height();
	while(counter --> 0) {
		std::vector<Chthon::Point> new_points;
		foreach(const Chthon::Point & current_pos, current_points) {
			if(current_pos == target) {
				if(player.pos != pos) {
					return false;
				}
				if(cells.cell(target).type != Cell::FLOOR && cells.cell(target).type != Cell::SLOT) {
					return false;
				}
				Chthon::Point current = current_pos;
				std::vector<Chthon::Point> path;
				path << current;
				int count = 2000;
				while(current != pos) {
					if(--count < 0) {
						break;
					}
					int current_cell = passed[current.x + current.y * width()];
					std::vector<Chthon::Point> neighs;
					neighs << current + Chthon::Point(1, 0) << current + Chthon::Point(-1, 0) << current + Chthon::Point(0, 1) << current + Chthon::Point(0, -1);
					Chthon::Point step;
					foreach(const Chthon::Point & neigh, neighs) {
						if(!isValid(neigh) || passed[neigh.x + neigh.y * width()] < 0) {
							continue;
						}
						int & neigh_cell = passed[neigh.x + neigh.y * width()];
						if(neigh_cell == current_cell - 1) {
							path << neigh;
							current = neigh;
							break;
						}
					}
				}
				std::reverse(path.begin(), path.end());
				for(unsigned i = 1; i < path.size(); ++i) {
					Chthon::Point r = path[i] - path[i - 1];
					if(r.x > 0) {
						movePlayer(Sokoban::RIGHT);
					} else if(r.x < 0) {
						movePlayer(Sokoban::LEFT);
					} else if(r.y > 0) {
						movePlayer(Sokoban::DOWN);
					} else if(r.y < 0) {
						movePlayer(Sokoban::UP);
					}
				}
				return true;
			}
			int current_cell = passed[current_pos.x + current_pos.y * width()];
			std::vector<Chthon::Point> neighs;
			for(unsigned i = 0; i < passed.size(); ++i) {
				neighs << current_pos + Chthon::Point(1, 0) << current_pos + Chthon::Point(-1, 0) << current_pos + Chthon::Point(0, 1) << current_pos + Chthon::Point(0, -1);
			}
			foreach(const Chthon::Point & neigh, neighs) {
				if(!isValid(neigh) || passed[neigh.x + neigh.y * width()] < 0) {
					continue;
				}
				int & neigh_cell = passed[neigh.x + neigh.y * width()];
				if(neigh_cell == 0) {
					neigh_cell = current_cell + 1;
					new_points << neigh;
				}
			}
		}
		if(new_points.empty()) {
			return false;
		}
		current_points = new_points;
	}
	return false;
}

void Sokoban::fillFloor(std::vector<int> & reachable, const Chthon::Point & point)
{
	if(!isValid(point)) {
		return;
	}
	if(reachable[point.x + point.y * width()] != 0) {
		return;
	}
	reachable[point.x + point.y * width()] = 2;
	fillFloor(reachable, point + Chthon::Point(0, 1));
	fillFloor(reachable, point + Chthon::Point(0, -1));
	fillFloor(reachable, point + Chthon::Point(1, 0));
	fillFloor(reachable, point + Chthon::Point(-1, 0));
}

Chthon::Point Sokoban::getPlayerPos() const
{
	return player.pos;
}

Cell Sokoban::getCellAt(int x, int y) const
{
	return cells.cell(x, y);
}

Object Sokoban::getObjectAt(int x, int y) const
{
	if(player.pos == Chthon::Point(x, y)) {
		return player;
	}
	foreach(const Object & box, boxes) {
		if(box.pos == Chthon::Point(x, y)) {
			return box;
		}
	}
	return Object();
}

bool Sokoban::isValid(const Chthon::Point & pos) const
{
	return cells.valid(pos);
}

bool Sokoban::runPlayer(int control)
{
	if(!valid) {
		return false;
	}
	bool moved = false;
	while(movePlayer(control, true)) {
		moved = true;
	}
	return moved;
}

bool Sokoban::movePlayer(int control, bool cautious)
{
	if(!valid) {
		return false;
	}
	std::map<int, std::pair<int, int> > diagonal_controls;
	diagonal_controls[UP_LEFT] = std::make_pair<int, int>(UP, LEFT);
	diagonal_controls[UP_RIGHT] = std::make_pair<int, int>(UP, RIGHT);
	diagonal_controls[DOWN_LEFT] = std::make_pair<int, int>(DOWN, LEFT);
	diagonal_controls[DOWN_RIGHT] = std::make_pair<int, int>(DOWN, RIGHT);
	std::map<int, Chthon::Point> shiftForControl;
	shiftForControl[UP] = Chthon::Point(0, -1);
	shiftForControl[DOWN] = Chthon::Point(0, 1);
	shiftForControl[RIGHT] = Chthon::Point(1, 0);
	shiftForControl[LEFT] = Chthon::Point(-1, 0);
	std::map<int, char> charForControl;
	charForControl[UP] = 'u';
	charForControl[DOWN] = 'd';
	charForControl[RIGHT] = 'r';
	charForControl[LEFT] = 'l';
	std::map<int, int> poseForControl;
	poseForControl[DOWN] = 0;
	poseForControl[LEFT] = 1;
	poseForControl[UP] = 2;
	poseForControl[RIGHT] = 3;

	if(diagonal_controls.count(control) > 0) {
		std::pair<int, int> controls = diagonal_controls[control];
		bool ok = false;
		ok = movePlayer(controls.first, true);
		if(ok) {
			ok = movePlayer(controls.second, true);
			if(!ok) {
				undo();
			}
		} else {
			ok = movePlayer(controls.second, true);
			if(ok) {
				ok = movePlayer(controls.first, true);
				if(!ok) {
					undo();
				}
			}
		}
		return ok;
	}

	Chthon::Point shift = shiftForControl[control];
	if(shift.null()) {
		return false;
	}
	Chthon::Point playerPos = getPlayerPos();
	Chthon::Point newPlayerPos = playerPos + shift;
	Chthon::Point newSecondPos = newPlayerPos + shift;
	if(!isValid(newPlayerPos)) {
		return false;
	}
	if(cells.cell(newPlayerPos).type == Cell::WALL) {
		return false;
	}
	if(has_box(newPlayerPos)) {
		if(cautious) {
			return false;
		}
		if(!isValid(newSecondPos)) {
			return false;
		}
		if(cells.cell(newSecondPos).type == Cell::WALL) {
			return false;
		}
		if(has_box(newSecondPos)) {
			return false;
		}
	}

	char controlChar = charForControl[control];
	player.pos = newPlayerPos;
	if(has_box(newPlayerPos)) {
		for(unsigned i = 0; i < boxes.size(); ++i) {
			if(boxes[i].pos == newPlayerPos) {
				boxes[i].pos = newSecondPos;
				break;
			}
		}
		controlChar = toupper(controlChar);
	}
	if(DIRECTIONAL_PLAYER_SPRITES) {
		player.sprite = poseForControl[control];
	}
	history.append(std::string(1, controlChar));
	return true;
}

std::string Sokoban::toString() const
{
	std::string result;
	for(int y = 0; y < height(); ++y) {
		for(int x = 0; x < width(); ++x) {
			Chthon::Point pos(x, y);
			bool is_player = player.pos == pos;
			bool is_box = has_box(pos);
			char ch = ' ';
			switch(cells.cell(pos).type) {
				case Cell::SPACE: ch = ' '; break;
				case Cell::FLOOR: ch = is_player ? '@' : (is_box ? '$' : ' '); break;
				case Cell::WALL: ch = '#'; break;
				case Cell::SLOT: ch = is_player ? '+' : (is_box ? '*' : '.'); break;
			}
			result += ch;
		}
		if(y != height() - 1) {
			result += '\n';
		}
	}
	return result;
}

std::string Sokoban::historyAsString() const
{
	return history;
}

bool Sokoban::undo()
{
	if(!valid) {
		return false;
	}
	std::map<char, Chthon::Point> shiftForControl;
	shiftForControl['u'] = Chthon::Point(0, -1);
	shiftForControl['d'] = Chthon::Point(0, 1);
	shiftForControl['r'] = Chthon::Point(1, 0);
	shiftForControl['l'] = Chthon::Point(-1, 0);
	std::map<char, int> poseForControl;
	poseForControl['d'] = 0;
	poseForControl['l'] = 1;
	poseForControl['u'] = 2;
	poseForControl['r'] = 3;

	std::string realHistory = history;
	if(fullHistoryTracking) {
		while(!realHistory.empty() && Chthon::ends_with(realHistory, "-")) {
			int count = 0;
			while(Chthon::ends_with(realHistory, "-")) {
				++count;
				realHistory.erase(realHistory.size() - 1, 1);
			}
			while(count > 0) {
				--count;
				realHistory.erase(realHistory.size() - 1, 1);
			}
		}
	}

	if(realHistory.empty()) {
		return false;
	}

	char control = realHistory.at(realHistory.size() - 1);
	static std::string possible_controls_str = "ulrdURLD";
	static std::set<char> possible_controls(possible_controls_str.begin(), possible_controls_str.end());
	if(possible_controls.count(control) == 0) {
		throw InvalidUndoException(control);
	}
	Chthon::Point shift = shiftForControl[tolower(control)];
	Chthon::Point playerPos = getPlayerPos();
	Chthon::Point oldPlayerPos = playerPos - shift;
	Chthon::Point boxPos = playerPos + shift;
	if(!isValid(oldPlayerPos)) {
		throw InvalidUndoException(control);
	}
	if(cells.cell(oldPlayerPos).type == Cell::WALL) {
		throw InvalidUndoException(control);
	}
	if(has_box(oldPlayerPos)) {
		throw InvalidUndoException(control);
	}
	if(isupper(control)) {
		if(!isValid(boxPos)) {
			throw InvalidUndoException(control);
		}
		if(!has_box(boxPos)) {
			throw InvalidUndoException(control);
		}
	}

	player.pos = oldPlayerPos;
	if(isupper(control)) {
		for(unsigned i = 0; i < boxes.size(); ++i) {
			if(boxes[i].pos == boxPos) {
				boxes[i].pos = playerPos;
				break;
			}
		}
	}
	if(DIRECTIONAL_PLAYER_SPRITES) {
		player.sprite = poseForControl[tolower(control)];
	}
	if(fullHistoryTracking) {
		history += '-';
	} else {
		history.erase(history.size() - 1, 1);
	}
	return true;
}

bool Sokoban::isSolved() const
{
	if(!valid) {
		return false;
	}
	int freeBoxCount = 0, freeSlotCount = 0;
	for(int y = 0; y < height(); ++y) {
		for(int x = 0; x < width(); ++x) {
			if(cells.cell(Chthon::Point(x, y)).type == Cell::SLOT && !has_box(Chthon::Point(x, y))) {
				freeSlotCount++;
			}
		}
	}
	foreach(const Object & box, boxes) {
		if(cells.cell(box.pos).type != Cell::SLOT) {
			freeBoxCount++;
		}
	}
	return (freeBoxCount == 0) && (freeSlotCount == 0);
}

