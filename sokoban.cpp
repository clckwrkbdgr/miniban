#include <QtDebug>
#include <QtCore/QStringList>
#include "sokoban.h"

const QChar FLOOR           = ' ';
const QChar WALL            = '#';
const QChar PLAYER_ON_FLOOR = '@';
const QChar EMPTY_SLOT      = '.';
const QChar PLAYER_ON_SLOT  = '+';
const QChar BOX_ON_FLOOR    = '$';
const QChar BOX_ON_SLOT     = '*';

const QChar UP         = 'u';
const QChar DOWN       = 'd';
const QChar LEFT       = 'l';
const QChar RIGHT      = 'r';
const QChar PUSH_UP    = 'U';
const QChar PUSH_DOWN  = 'D';
const QChar PUSH_LEFT  = 'L';
const QChar PUSH_RIGHT = 'R';

int findPlayerPos(const QString & field)
{
	int playerCount = field.count(PLAYER_ON_FLOOR) + field.count(PLAYER_ON_SLOT);
	if(playerCount != 1) {
		throw Sokoban::InvalidPlayerCountException(playerCount);
	}
	int playerPos = field.indexOf(PLAYER_ON_FLOOR);
	if(playerPos < 0) {
		playerPos = field.indexOf(PLAYER_ON_SLOT);
	}
	return playerPos;
}

int getOneLineUpperPos(const QString & field, int playerPos)
{
	int currentLineStart = field.lastIndexOf('\n', playerPos);
	int prevLineStart = field.lastIndexOf('\n', currentLineStart - 1);
	int relPlayerPos = playerPos - currentLineStart;
	return (prevLineStart + relPlayerPos);
}

int getOneLineLowerPos(const QString & field, int playerPos)
{
	int currentLineStart = field.lastIndexOf('\n', playerPos);
	int nextLineStart = field.indexOf('\n', currentLineStart + 1);
	int relPlayerPos = playerPos - currentLineStart;
	return (nextLineStart + relPlayerPos);
}

bool isFreeToMove(const QString & field, int pos)
{
	return field[pos] != WALL;
}

QChar getFloorSprite(const QString & field, int pos)
{
	if(field[pos] == PLAYER_ON_FLOOR) {
		return FLOOR;
	} else if(field[pos] == PLAYER_ON_SLOT) {
		return EMPTY_SLOT;
	} else if(field[pos] == BOX_ON_FLOOR) {
		return FLOOR;
	} else if(field[pos] == BOX_ON_SLOT) {
		return EMPTY_SLOT;
	}
	return field[pos];
}

QChar getPlayerSprite(const QString & field, int pos)
{
	if(field[pos] == FLOOR) {
		return PLAYER_ON_FLOOR;
	} else if(field[pos] == EMPTY_SLOT) {
		return PLAYER_ON_SLOT;
	}
	return field[pos];
}

QChar getBoxSprite(const QString & field, int pos)
{
	if(field[pos] == FLOOR) {
		return BOX_ON_FLOOR;
	} else if(field[pos] == EMPTY_SLOT) {
		return BOX_ON_SLOT;
	}
	return field[pos];
}

bool isBox(const QString & field, int pos)
{
	return (field[pos] == BOX_ON_FLOOR || field[pos] == BOX_ON_SLOT);
}

int getNewPos(const QString & field, int pos, const QChar & control)
{
	int currentLineStart = field.lastIndexOf('\n', pos);
	int nextLineStart = field.indexOf('\n', currentLineStart + 1);
	int prevLineStart = field.lastIndexOf('\n', currentLineStart - 1);
	int relPos = pos - currentLineStart;

	if(control == RIGHT) {
		bool isLastLine = field.indexOf('\n', pos) < 0;
		bool crossRightBorder = isLastLine ? (pos + 1 >= field.size()) : (pos + 1 >= nextLineStart);
		if(crossRightBorder)
			throw Sokoban::OutOfMapException();
		return pos + 1;
	} else if(control == LEFT) {
		bool isFirstLine = field.lastIndexOf('\n', pos) < 0;
		bool crossLeftBorder = isFirstLine ? (pos - 1 < 0) : (pos - 1 <= currentLineStart);
		if(crossLeftBorder)
			throw Sokoban::OutOfMapException();
		return pos - 1;
	} else if(control == DOWN) {
		if(nextLineStart < 0)
			throw Sokoban::OutOfMapException();
		return nextLineStart + relPos;
	} else if(control == UP) {
		if(currentLineStart < 0)
			throw Sokoban::OutOfMapException();
		return prevLineStart + relPos;
	}
	throw Sokoban::InvalidControlException(control);
}

QString Sokoban::process(const QString & field, const QChar & control)
{
	QString result = field;
	int playerPos = findPlayerPos(field);
	int oldPlayerPos = playerPos;
	int newPlayerPos = getNewPos(field, oldPlayerPos, control);
	if(isFreeToMove(field, newPlayerPos)) {
		if(isBox(field, newPlayerPos)) {
			int oldBoxPos = newPlayerPos;
			int newBoxPos = getNewPos(field, oldBoxPos, control);
			if(isFreeToMove(field, newBoxPos) && !isBox(field, newBoxPos)) {
				result[oldBoxPos] = getFloorSprite(field, oldBoxPos);
				result[newBoxPos] = getBoxSprite(field, newBoxPos);
			} else {
				return result;
			}
		}

		result[oldPlayerPos] = getFloorSprite(result, oldPlayerPos);
		result[newPlayerPos] = getPlayerSprite(result, newPlayerPos);
	}
	return result;
}

QString Sokoban::undo(const QString & field, QString * history)
{
	if(history == NULL || history->isEmpty())
		return field;

	QChar control = history->at(history->size() - 1);
	QChar reversedControl;
	bool withPush = false;
	if(control == 'u') {
		reversedControl = 'd';
	} else if(control == 'r') {
		reversedControl = 'l';
	} else if(control == 'd') {
		reversedControl = 'u';
	} else if(control == 'l') {
		reversedControl = 'r';
	} else if(control == 'U') {
		withPush = true;
		reversedControl = 'd';
	} else if(control == 'D') {
		withPush = true;
		reversedControl = 'u';
	} else if(control == 'L') {
		withPush = true;
		reversedControl = 'r';
	} else if(control == 'R') {
		withPush = true;
		reversedControl = 'l';
	} else {
		throw Sokoban::InvalidUndoException(control);
	}

	QString result = field;
	int playerPos = findPlayerPos(field);
	int oldPlayerPos = playerPos;
	int newPlayerPos = getNewPos(field, oldPlayerPos, reversedControl);
	if(isFreeToMove(field, newPlayerPos)) {
		result[oldPlayerPos] = getFloorSprite(result, oldPlayerPos);
		result[newPlayerPos] = getPlayerSprite(result, newPlayerPos);

		if(withPush) {
			QChar boxControl = control.toLower();
			int newBoxPos = oldPlayerPos;
			int oldBoxPos = getNewPos(result, newBoxPos, boxControl);
			result[oldBoxPos] = getFloorSprite(result, oldBoxPos);
			result[newBoxPos] = getBoxSprite(result, newBoxPos);
		}
	} else {
		throw Sokoban::InvalidUndoException(control);
	}
	history->remove(history->size() - 1, 1);
	return result;
}

bool Sokoban::isSolved(const QString & field)
{
	int freeSlotCount = field.count(EMPTY_SLOT) + field.count(PLAYER_ON_SLOT);
	int freeBoxCount =  field.count(BOX_ON_FLOOR);
	return (freeSlotCount == 0) && (freeBoxCount == 0);
}
