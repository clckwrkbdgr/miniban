#include <QtDebug>
#include <QtCore/QStringList>
#include "sokoban.h"

Sokoban::Sokoban(const QString & levelField, const QString & backgroundHistory, bool isFullHistoryTracked)
	: history(backgroundHistory), fullHistoryTracking(isFullHistoryTracked)
{
	QStringList rows = levelField.split('\n');
	size.setHeight(rows.count());
	foreach(const QString & row, rows) {
		if(size.width() < row.length()) {
			size.setWidth(row.length());
		}
	}

	QMap<QChar, int> charToCell;
	charToCell[' '] = SPACE;
	charToCell['#'] = WALL;
	charToCell['@'] = SPACE;
	charToCell['.'] = EMPTY_SLOT;
	charToCell['+'] = EMPTY_SLOT;
	charToCell['$'] = FLOOR;
	charToCell['*'] = EMPTY_SLOT;
	QString validChars;
	foreach(const QChar & ch, charToCell.keys()) {
		validChars.append(ch);
	}

	cells.fill(Sokoban::SPACE, size.width() * size.height());
	player = QPoint(-1, -1);
	int playerCount = 0;
	for(int y = 0; y < rows.count(); ++y) {
		const QString & row = rows[y];
		for(int x = 0; x < row.size(); ++x) {
			if(validChars.contains(row[x])) {
				cell(QPoint(x, y)) = charToCell[row[x]];
				if(row[x] == '$' || row[x] == '*') {
					boxes << QPoint(x, y);
				}
				if(row[x] == '@' || row[x] == '+') {
					if(player.x() < 0) {
						player = QPoint(x, y);
					}
					playerCount++;
				}
			} else {
				cell(QPoint(x, y)) = WALL;
			}
		}
	}
	if(playerCount != 1) {
		throw InvalidPlayerCountException(playerCount);
	}

	// 0 - passable, 1 - impassable, 2 - found to be floor.
	QVector<int> reachable(cells.size(), 0);
	for(int i = 0; i < cells.size(); ++i) {
		reachable[i] = (cells[i] == WALL) ? 1 : 0;
	}
	fillFloor(reachable, getPlayerPos());
	for(int i = 0; i < cells.size(); ++i) {
		if(reachable[i] == 2 && cells[i] == SPACE) {
			cells[i] = FLOOR;
		}
	}
}

bool Sokoban::movePlayer(const QPoint & target)
{
	QVector<int> passed(cells.size(), -1);
	for(int i = 0; i < cells.size(); ++i) {
		if(cells[i] != WALL && !boxes.contains(QPoint(i % width(), i / width()))) {
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
				if(player != pos) {
					return false;
				}
				if(cell(target) != FLOOR && cell(target) != EMPTY_SLOT) {
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
	return player;
}

const Sokoban::Cell & Sokoban::cell(const QPoint & point) const
{
	return cells[point.x() + point.y() * width()];
}

Sokoban::Cell & Sokoban::cell(const QPoint & point)
{
	return cells[point.x() + point.y() * width()];
}

Sokoban::Sprite Sokoban::getCellSprite(const QPoint & point) const
{
	switch(cell(point)) {
		case FLOOR: return FLOOR;
		case SPACE: return SPACE;
		case WALL: return WALL;
		case PLAYER_ON_FLOOR: return FLOOR;
		case EMPTY_SLOT: return EMPTY_SLOT;
		case PLAYER_ON_SLOT: return EMPTY_SLOT;
		case BOX_ON_FLOOR: return FLOOR;
		case BOX_ON_SLOT: return EMPTY_SLOT;
		default: return SPACE;
	}
}

Sokoban::Sprite Sokoban::getObjectSprite(const QPoint & point) const
{
	if(point == player) {
		switch(cell(point)) {
			case FLOOR: return PLAYER_ON_FLOOR;
			case EMPTY_SLOT: return PLAYER_ON_SLOT;
			default: return NONE;
		}
	}
	if(boxes.contains(point)) {
		switch(cell(point)) {
			case FLOOR: return BOX_ON_FLOOR;
			case EMPTY_SLOT: return BOX_ON_SLOT;
			default: return NONE;
		}
	}
	switch(cell(point)) {
		case FLOOR: return NONE;
		case SPACE: return NONE;
		case WALL: return NONE;
		case PLAYER_ON_FLOOR: return PLAYER_ON_FLOOR;
		case EMPTY_SLOT: return NONE;
		case PLAYER_ON_SLOT: return PLAYER_ON_SLOT;
		case BOX_ON_FLOOR: return BOX_ON_FLOOR;
		case BOX_ON_SLOT: return BOX_ON_SLOT;
		default: return SPACE;
	}
}

bool Sokoban::isValid(const QPoint & pos) const
{
	return (pos.x() >= 0 && pos.x() < width() && pos.y() >= 0 && pos.y() < height());
}

bool Sokoban::runPlayer(int control)
{
	bool moved = false;
	while(movePlayer(control, true)) {
		moved = true;
	}
	return moved;
}

bool Sokoban::movePlayer(int control, bool cautious)
{
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

	QPoint shift = shiftForControl[control];
	if(shift.isNull()) {
		return false;
	}
	QPoint playerPos = getPlayerPos();
	QPoint newPlayerPos = playerPos + shift;
	QPoint newSecondPos = newPlayerPos + shift;
	if(!isValid(newPlayerPos)) {
		throw OutOfMapException();
	}
	if(cell(newPlayerPos) == WALL) {
		return false;
	}
	if(boxes.contains(newPlayerPos)) {
		if(cautious) {
			return false;
		}
		if(!isValid(newSecondPos)) {
			return false;
		}
		if(cell(newSecondPos) == WALL) {
			return false;
		}
		if(boxes.contains(newSecondPos)) {
			return false;
		}
	}

	QChar controlChar = charForControl[control];
	player = newPlayerPos;
	if(boxes.contains(newPlayerPos)) {
		boxes[boxes.indexOf(newPlayerPos)] = newSecondPos;
		controlChar = controlChar.toUpper();
	}
	history.append(controlChar);
	return true;
}

QString Sokoban::toString() const
{
	QMap<QChar, int> cellToChar;
	cellToChar[FLOOR          ] = ' ';
	cellToChar[SPACE          ] = ' ';
	cellToChar[WALL           ] = '#';
	cellToChar[PLAYER_ON_FLOOR] = '@';
	cellToChar[EMPTY_SLOT     ] = '.';
	cellToChar[PLAYER_ON_SLOT ] = '+';
	cellToChar[BOX_ON_FLOOR   ] = '$';
	cellToChar[BOX_ON_SLOT    ] = '*';

	QString result;
	for(int y = 0; y < size.height(); ++y) {
		for(int x = 0; x < size.width(); ++x) {
			QChar ch = cellToChar[cells[x + y * size.width()]];
			if(player == QPoint(x, y)) {
				if(ch == ' ') {
					ch = '@';
				} else if(ch == '.') {
					ch = '+';
				}
			}
			if(boxes.contains(QPoint(x, y))) {
				if(ch == ' ') {
					ch = '$';
				} else if(ch == '.') {
					ch = '*';
				}
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
	QMap<QChar, QPoint> shiftForControl;
	shiftForControl['u'] = QPoint(0, -1);
	shiftForControl['d'] = QPoint(0, 1);
	shiftForControl['r'] = QPoint(1, 0);
	shiftForControl['l'] = QPoint(-1, 0);

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
	if(cell(oldPlayerPos) == WALL) {
		throw InvalidUndoException(control);
	}
	if(boxes.contains(oldPlayerPos)) {
		throw InvalidUndoException(control);
	}
	if(control.isUpper()) {
		if(!isValid(boxPos)) {
			throw InvalidUndoException(control);
		}
		if(!boxes.contains(boxPos)) {
			throw InvalidUndoException(control);
		}
	}

	player = oldPlayerPos;
	if(control.isUpper()) {
		boxes[boxes.indexOf(boxPos)] = playerPos;
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
	int freeBoxCount = 0, freeSlotCount = 0;
	for(int y = 0; y < size.height(); ++y) {
		for(int x = 0; x < size.width(); ++x) {
			if(cell(QPoint(x, y)) == EMPTY_SLOT && !boxes.contains(QPoint(x, y))) {
				freeSlotCount++;
			}
		}
	}
	foreach(const QPoint & box, boxes) {
		if(cell(box) != EMPTY_SLOT) {
			freeBoxCount++;
		}
	}
	return (freeBoxCount == 0) && (freeSlotCount == 0);
}

