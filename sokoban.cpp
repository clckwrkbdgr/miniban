#include <QtDebug>
#include <QtCore/QStringList>
#include "sokoban.h"

static const bool DIRECTIONAL_PLAYER_SPRITES = false;

Sokoban::Sokoban()
	: valid(false)
{
}

Sokoban::Sokoban(const QString & levelField, const QString & backgroundHistory, bool isFullHistoryTracked)
	: valid(true), history(backgroundHistory), fullHistoryTracking(isFullHistoryTracked)
{
	QStringList rows = levelField.split('\n');
	size.setHeight(rows.count());
	foreach(const QString & row, rows) {
		if(size.width() < row.length()) {
			size.setWidth(row.length());
		}
	}

	cells.fill(Cell(), size.width() * size.height());
	player = Object();
	int playerCount = 0;
	for(int y = 0; y < rows.count(); ++y) {
		const QString & row = rows[y];
		for(int x = 0; x < row.size(); ++x) {
			QPoint pos(x, y);
			switch(row[x].toAscii()) {
				case ' ': cell(pos).type = Cell::SPACE; break;
				case '#': cell(pos).type = Cell::WALL; break;
				case '@': cell(pos).type = Cell::SPACE; player = Object(pos, true); ++playerCount; break;
				case '.': cell(pos).type = Cell::SLOT; break;
				case '+': cell(pos).type = Cell::SLOT; player = Object(pos, true); ++playerCount; break;
				case '$': cell(pos).type = Cell::SPACE; boxes << Object(pos); break;
				case '*': cell(pos).type = Cell::SLOT; boxes << Object(pos); break;
			}
			int sprite_chance = qrand() % 100;
			if(sprite_chance < 50) {
				cell(pos).sprite = 0;
			} else if(sprite_chance < 80) {
				cell(pos).sprite = 1;
			} else if(sprite_chance < 95) {
				cell(pos).sprite = 2;
			} else {
				cell(pos).sprite = 3;
			}
		}
	}
	if(playerCount != 1) {
		throw InvalidPlayerCountException(playerCount);
	}
	for(int i = 0; i < boxes.count(); ++i) {
		boxes[i].sprite = qrand() % 4;
	}

	// 0 - passable, 1 - impassable, 2 - found to be floor.
	QVector<int> reachable(cells.size(), 0);
	for(int i = 0; i < cells.size(); ++i) {
		reachable[i] = (cells[i].type == Cell::WALL) ? 1 : 0;
	}
	fillFloor(reachable, getPlayerPos());
	for(int i = 0; i < cells.size(); ++i) {
		if(reachable[i] == 2 && cells[i].type == Cell::SPACE) {
			cells[i].type = Cell::FLOOR;
		}
	}
}

void Sokoban::restart()
{
	while(undo()) {
	}
}

bool Sokoban::has_box(const QPoint & point) const
{
	foreach(const Object & box, boxes) {
		if(box.pos == point) {
			return true;
		}
	}
	return false;
}

bool Sokoban::movePlayer(const QPoint & target)
{
	if(!valid) {
		return false;
	}
	QVector<int> passed(cells.size(), -1);
	for(int i = 0; i < cells.size(); ++i) {
		if(cells[i].type != Cell::WALL && !has_box(QPoint(i % width(), i / width()))) {
			passed[i] = 0;
		}
	}
	QPoint pos = getPlayerPos();
	if(pos == target) {
		return true;
	}
	passed[pos.x() + pos.y() * width()] = 1;
	QList<QPoint> current_points;
	current_points << pos;
	for(int counter = 0; counter < cells.size(); ++counter) {
		QList<QPoint> new_points;
		foreach(const QPoint & current_pos, current_points) {
			if(current_pos == target) {
				if(player.pos != pos) {
					return false;
				}
				if(cell(target).type != Cell::FLOOR && cell(target).type != Cell::SLOT) {
					return false;
				}
				QPoint current = current_pos;
				QList<QPoint> path;
				path << current;
				int count = 2000;
				while(current != pos) {
					if(--count < 0) {
						break;
					}
					int current_cell = passed[current.x() + current.y() * width()];
					QList<QPoint> neighs;
					for(int i = 0; i < passed.size(); ++i) {
						neighs << current + QPoint(1, 0) << current + QPoint(-1, 0) << current + QPoint(0, 1) << current + QPoint(0, -1);
					}
					QPoint step;
					foreach(const QPoint & neigh, neighs) {
						if(!isValid(neigh) || passed[neigh.x() + neigh.y() * width()] < 0) {
							continue;
						}
						int & neigh_cell = passed[neigh.x() + neigh.y() * width()];
						if(neigh_cell == current_cell - 1) {
							path << neigh;
							current = neigh;
							break;
						}
					}
				}
				for(int k = 0; k < (path.size() / 2); ++k) {
					path.swap(k, path.size() - (1 + k));
				}
				for(int i = 1; i < path.size(); ++i) {
					QPoint r = path[i] - path[i - 1];
					if(r.x() > 0) {
						movePlayer(Sokoban::RIGHT);
					} else if(r.x() < 0) {
						movePlayer(Sokoban::LEFT);
					} else if(r.y() > 0) {
						movePlayer(Sokoban::DOWN);
					} else if(r.y() < 0) {
						movePlayer(Sokoban::UP);
					}
				}
				return true;
			}
			int current_cell = passed[current_pos.x() + current_pos.y() * width()];
			QList<QPoint> neighs;
			for(int i = 0; i < passed.size(); ++i) {
				neighs << current_pos + QPoint(1, 0) << current_pos + QPoint(-1, 0) << current_pos + QPoint(0, 1) << current_pos + QPoint(0, -1);
			}
			foreach(const QPoint & neigh, neighs) {
				if(!isValid(neigh) || passed[neigh.x() + neigh.y() * width()] < 0) {
					continue;
				}
				int & neigh_cell = passed[neigh.x() + neigh.y() * width()];
				if(neigh_cell == 0) {
					neigh_cell = current_cell + 1;
					new_points << neigh;
				}
			}
		}
		if(new_points.isEmpty()) {
			return false;
		}
		current_points = new_points;
	}
	return false;
}

void Sokoban::fillFloor(QVector<int> & reachable, const QPoint & point)
{
	if(!isValid(point)) {
		return;
	}
	if(reachable[point.x() + point.y() * width()] != 0) {
		return;
	}
	reachable[point.x() + point.y() * width()] = 2;
	fillFloor(reachable, point + QPoint(0, 1));
	fillFloor(reachable, point + QPoint(0, -1));
	fillFloor(reachable, point + QPoint(1, 0));
	fillFloor(reachable, point + QPoint(-1, 0));
}

QPoint Sokoban::getPlayerPos() const
{
	return player.pos;
}

const Cell & Sokoban::cell(const QPoint & point) const
{
	return cells[point.x() + point.y() * width()];
}

Cell & Sokoban::cell(const QPoint & point)
{
	return cells[point.x() + point.y() * width()];
}

Cell Sokoban::getCellAt(int x, int y) const
{
	if(!isValid(QPoint(x, y))) {
		return Cell();
	}
	return cell(QPoint(x, y));
}

Object Sokoban::getObjectAt(int x, int y) const
{
	if(player.pos == QPoint(x, y)) {
		return player;
	}
	foreach(const Object & box, boxes) {
		if(box.pos == QPoint(x, y)) {
			return box;
		}
	}
	return Object();
}

bool Sokoban::isValid(const QPoint & pos) const
{
	if(!valid) {
		return false;
	}
	return (pos.x() >= 0 && pos.x() < width() && pos.y() >= 0 && pos.y() < height());
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
	QMap<int, QPair<int, int> > diagonal_controls;
	diagonal_controls[UP_LEFT] = qMakePair<int, int>(UP, LEFT);
	diagonal_controls[UP_RIGHT] = qMakePair<int, int>(UP, RIGHT);
	diagonal_controls[DOWN_LEFT] = qMakePair<int, int>(DOWN, LEFT);
	diagonal_controls[DOWN_RIGHT] = qMakePair<int, int>(DOWN, RIGHT);
	QMap<int, QPoint> shiftForControl;
	shiftForControl[UP] = QPoint(0, -1);
	shiftForControl[DOWN] = QPoint(0, 1);
	shiftForControl[RIGHT] = QPoint(1, 0);
	shiftForControl[LEFT] = QPoint(-1, 0);
	QMap<int, QChar> charForControl;
	charForControl[UP] = 'u';
	charForControl[DOWN] = 'd';
	charForControl[RIGHT] = 'r';
	charForControl[LEFT] = 'l';
	QMap<int, int> poseForControl;
	poseForControl[DOWN] = 0;
	poseForControl[LEFT] = 1;
	poseForControl[UP] = 2;
	poseForControl[RIGHT] = 3;

	if(diagonal_controls.contains(control)) {
		QPair<int, int> controls = diagonal_controls[control];
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

	QPoint shift = shiftForControl[control];
	if(shift.isNull()) {
		return false;
	}
	QPoint playerPos = getPlayerPos();
	QPoint newPlayerPos = playerPos + shift;
	QPoint newSecondPos = newPlayerPos + shift;
	if(!isValid(newPlayerPos)) {
		return false;
	}
	if(cell(newPlayerPos).type == Cell::WALL) {
		return false;
	}
	if(has_box(newPlayerPos)) {
		if(cautious) {
			return false;
		}
		if(!isValid(newSecondPos)) {
			return false;
		}
		if(cell(newSecondPos).type == Cell::WALL) {
			return false;
		}
		if(has_box(newSecondPos)) {
			return false;
		}
	}

	QChar controlChar = charForControl[control];
	player.pos = newPlayerPos;
	if(has_box(newPlayerPos)) {
		for(int i = 0; i < boxes.size(); ++i) {
			if(boxes[i].pos == newPlayerPos) {
				boxes[i].pos = newSecondPos;
				break;
			}
		}
		controlChar = controlChar.toUpper();
	}
	if(DIRECTIONAL_PLAYER_SPRITES) {
		player.sprite = poseForControl[control];
	}
	history.append(controlChar);
	return true;
}

QString Sokoban::toString() const
{
	QString result;
	for(int y = 0; y < size.height(); ++y) {
		for(int x = 0; x < size.width(); ++x) {
			QPoint pos(x, y);
			bool is_player = player.pos == pos;
			bool is_box = has_box(pos);
			QChar ch = ' ';
			switch(cell(pos).type) {
				case Cell::SPACE: ch = ' '; break;
				case Cell::FLOOR: ch = is_player ? '@' : (is_box ? '$' : ' '); break;
				case Cell::WALL: ch = '#'; break;
				case Cell::SLOT: ch = is_player ? '+' : (is_box ? '*' : '.'); break;
			}
			result.append(ch);
		}
		if(y != size.height() - 1) {
			result.append('\n');
		}
	}
	return result;
}

QString Sokoban::historyAsString() const
{
	return history;
}

bool Sokoban::undo()
{
	if(!valid) {
		return false;
	}
	QMap<QChar, QPoint> shiftForControl;
	shiftForControl['u'] = QPoint(0, -1);
	shiftForControl['d'] = QPoint(0, 1);
	shiftForControl['r'] = QPoint(1, 0);
	shiftForControl['l'] = QPoint(-1, 0);
	QMap<QChar, int> poseForControl;
	poseForControl['d'] = 0;
	poseForControl['l'] = 1;
	poseForControl['u'] = 2;
	poseForControl['r'] = 3;

	QString realHistory = history;
	if(fullHistoryTracking) {
		while(!realHistory.isEmpty() && realHistory.endsWith('-')) {
			int count = 0;
			while(realHistory.endsWith('-')) {
				++count;
				realHistory.remove(realHistory.size() - 1, 1);
			}
			while(count > 0) {
				--count;
				realHistory.remove(realHistory.size() - 1, 1);
			}
		}
	}

	if(realHistory.isEmpty()) {
		return false;
	}

	QChar control = realHistory.at(realHistory.size() - 1);
	if(!QString("ulrdURLD").contains(control)) {
		throw InvalidUndoException(control);
	}
	QPoint shift = shiftForControl[control.toLower()];
	QPoint playerPos = getPlayerPos();
	QPoint oldPlayerPos = playerPos - shift;
	QPoint boxPos = playerPos + shift;
	if(!isValid(oldPlayerPos)) {
		throw InvalidUndoException(control);
	}
	if(cell(oldPlayerPos).type == Cell::WALL) {
		throw InvalidUndoException(control);
	}
	if(has_box(oldPlayerPos)) {
		throw InvalidUndoException(control);
	}
	if(control.isUpper()) {
		if(!isValid(boxPos)) {
			throw InvalidUndoException(control);
		}
		if(!has_box(boxPos)) {
			throw InvalidUndoException(control);
		}
	}

	player.pos = oldPlayerPos;
	if(control.isUpper()) {
		for(int i = 0; i < boxes.size(); ++i) {
			if(boxes[i].pos == boxPos) {
				boxes[i].pos = playerPos;
				break;
			}
		}
	}
	if(DIRECTIONAL_PLAYER_SPRITES) {
		player.sprite = poseForControl[control.toLower()];
	}
	if(fullHistoryTracking) {
		history.append('-');
	} else {
		history.remove(history.size() - 1, 1);
	}
	return true;
}

bool Sokoban::isSolved()
{
	if(!valid) {
		return false;
	}
	int freeBoxCount = 0, freeSlotCount = 0;
	for(int y = 0; y < size.height(); ++y) {
		for(int x = 0; x < size.width(); ++x) {
			if(cell(QPoint(x, y)).type == Cell::SLOT && !has_box(QPoint(x, y))) {
				freeSlotCount++;
			}
		}
	}
	foreach(const Object & box, boxes) {
		if(cell(box.pos).type != Cell::SLOT) {
			freeBoxCount++;
		}
	}
	return (freeBoxCount == 0) && (freeSlotCount == 0);
}

