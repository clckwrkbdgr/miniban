#include <QtDebug>
#include <QtCore/QStringList>
#include "sokoban.h"

/*

namespace { // Auxiliary functions.

int findPlayerPos(const QString & field)
{
	int playerCount = field.count(Sokoban::PLAYER_ON_FLOOR) + field.count(Sokoban::PLAYER_ON_SLOT);
	if(playerCount != 1) {
		throw Sokoban::InvalidPlayerCountException(playerCount);
	}
	int playerPos = field.indexOf(Sokoban::PLAYER_ON_FLOOR);
	if(playerPos < 0) {
		playerPos = field.indexOf(Sokoban::PLAYER_ON_SLOT);
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

QChar getFloorSprite(const QString & field, int pos)
{
	if(field[pos] == Sokoban::PLAYER_ON_FLOOR) {
		return Sokoban::FLOOR;
	} else if(field[pos] == Sokoban::PLAYER_ON_SLOT) {
		return Sokoban::EMPTY_SLOT;
	} else if(field[pos] == Sokoban::BOX_ON_FLOOR) {
		return Sokoban::FLOOR;
	} else if(field[pos] == Sokoban::BOX_ON_SLOT) {
		return Sokoban::EMPTY_SLOT;
	}
	return field[pos];
}

QChar getPlayerSprite(const QString & field, int pos)
{
	if(field[pos] == Sokoban::FLOOR) {
		return Sokoban::PLAYER_ON_FLOOR;
	} else if(field[pos] == Sokoban::EMPTY_SLOT) {
		return Sokoban::PLAYER_ON_SLOT;
	}
	return field[pos];
}

QChar getBoxSprite(const QString & field, int pos)
{
	if(field[pos] == Sokoban::FLOOR) {
		return Sokoban::BOX_ON_FLOOR;
	} else if(field[pos] == Sokoban::EMPTY_SLOT) {
		return Sokoban::BOX_ON_SLOT;
	}
	return field[pos];
}

bool isBox(const QString & field, int pos)
{
	return (field[pos] == Sokoban::BOX_ON_FLOOR || field[pos] == Sokoban::BOX_ON_SLOT);
}

int getNewPos(const QString & field, int pos, const QChar & control)
{
	int currentLineStart = field.lastIndexOf('\n', pos);
	int nextLineStart = field.indexOf('\n', currentLineStart + 1);
	int prevLineStart = field.lastIndexOf('\n', currentLineStart - 1);
	int relPos = pos - currentLineStart;

	//# Player can move in four directions (up, down, left, right).
	if(control == Sokoban::RIGHT) {
		bool isLastLine = field.indexOf('\n', pos) < 0;
		bool crossRightBorder = isLastLine ? (pos + 1 >= field.size()) : (pos + 1 >= nextLineStart);
		if(crossRightBorder)
			throw Sokoban::OutOfMapException();
		return pos + 1;
	} else if(control == Sokoban::LEFT) {
		bool isFirstLine = field.lastIndexOf('\n', pos) < 0;
		bool crossLeftBorder = isFirstLine ? (pos - 1 < 0) : (pos - 1 <= currentLineStart);
		if(crossLeftBorder)
			throw Sokoban::OutOfMapException();
		return pos - 1;
	} else if(control == Sokoban::DOWN) {
		if(nextLineStart < 0)
			throw Sokoban::OutOfMapException();
		return nextLineStart + relPos;
	} else if(control == Sokoban::UP) {
		if(currentLineStart < 0)
			throw Sokoban::OutOfMapException();
		return prevLineStart + relPos;
	}
	throw Sokoban::InvalidControlException(control);
}

bool isFreeToMove(const QString & field, int pos)
{
	//# Player cannot move through walls.
	return field[pos] != Sokoban::WALL;
}

};

QString process(const QString & field, const QChar & control, QString * history)
{
	QString result = field;
	int playerPos = findPlayerPos(field);
	int oldPlayerPos = playerPos;
	int newPlayerPos = getNewPos(field, oldPlayerPos, control);
	bool withPush = false;
	if(isFreeToMove(field, newPlayerPos)) {
		//# Player can push box if there is a free cell behind it.
		if(isBox(field, newPlayerPos)) {
			int oldBoxPos = newPlayerPos;
			int newBoxPos = getNewPos(field, oldBoxPos, control);
			//# Only one box can be pushed at a time.
			//# Boxes cannot be pulled.
			if(isFreeToMove(field, newBoxPos) && !isBox(field, newBoxPos)) {
				result[oldBoxPos] = getFloorSprite(field, oldBoxPos);
				result[newBoxPos] = getBoxSprite(field, newBoxPos);
				withPush = true;
			} else {
				return result;
			}
		}

		result[oldPlayerPos] = getFloorSprite(result, oldPlayerPos);
		result[newPlayerPos] = getPlayerSprite(result, newPlayerPos);
		if(history) {
			*history += withPush ? control.toUpper() : control;
		}
	}
	return result;
}

QString undo(const QString & field, QString * history)
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

bool isSolved(const QString & field)
{
	//# The goal of the game is to move all the boxes into all slots.
	int freeSlotCount = field.count(Sokoban::EMPTY_SLOT) + field.count(Sokoban::PLAYER_ON_SLOT);
	int freeBoxCount =  field.count(Sokoban::BOX_ON_FLOOR);
	return (freeSlotCount == 0) && (freeBoxCount == 0);
}

*/

Sokoban::Sokoban(const QString & levelField, const QString & backgroundHistory)
	: history(backgroundHistory)
{
	QStringList rows = levelField.split('\n');
	size.setHeight(rows.count());
	foreach(const QString & row, rows) {
		if(size.width() < row.length()) {
			size.setWidth(row.length());
		}
	}

	QMap<QChar, int> charToCell;
	charToCell[' '] = FLOOR;
	charToCell['#'] = WALL;
	charToCell['@'] = PLAYER_ON_FLOOR;
	charToCell['.'] = EMPTY_SLOT;
	charToCell['+'] = PLAYER_ON_SLOT;
	charToCell['$'] = BOX_ON_FLOOR;
	charToCell['*'] = BOX_ON_SLOT;
	QString validChars;
	foreach(const QChar & ch, charToCell.keys()) {
		validChars.append(ch);
	}

	cells.fill(Sokoban::FLOOR, size.width() * size.height());
	for(int y = 0; y < rows.count(); ++y) {
		const QString & row = rows[y];
		for(int x = 0; x < row.size(); ++x) {
			if(validChars.contains(row[x])) {
				cell(QPoint(x, y)) = charToCell[row[x]];
			} else {
				cell(QPoint(x, y)) = WALL;
			}
		}
	}
}

QPoint Sokoban::getPlayerPos() const
{
	for(int x = 0; x < width(); ++x) {
		for(int y = 0; y < height(); ++y) {
			if(cell(QPoint(x, y)) == PLAYER_ON_FLOOR || cell(QPoint(x, y)) == PLAYER_ON_SLOT) {
				return QPoint(x, y);
			}
		}
	}
	return QPoint(-1, -1);
}

const Sokoban::Cell & Sokoban::cell(const QPoint & point) const
{
	return cells[point.x() + point.y() * width()];
}

Sokoban::Cell & Sokoban::cell(const QPoint & point)
{
	return cells[point.x() + point.y() * width()];
}

bool Sokoban::isValid(const QPoint & pos) const
{
	return (pos.x() >= 0 || pos.x() < width() || pos.y() >= 0 || pos.y() < height());
}

void Sokoban::processControls(const QString & controls)
{
	QMap<QChar, QPoint> shiftForControl;
	shiftForControl['u'] = QPoint(0, -1);
	shiftForControl['d'] = QPoint(0, 1);
	shiftForControl['r'] = QPoint(1, 0);
	shiftForControl['l'] = QPoint(-1, 0);
	foreach(QChar control, controls) {
		control = control.toLower();
		QPoint shift = shiftForControl[control];
		if(shift.isNull()) {
			continue;
		}
		QPoint playerPos = getPlayerPos();
		QPoint newPlayerPos = playerPos + shift;
		QPoint newSecondPos = newPlayerPos + shift;
		if(!isValid(newPlayerPos)) {
			continue;
		}
		if(cell(newPlayerPos) == WALL) {
			continue;
		}
		if(cell(newPlayerPos) == BOX_ON_SLOT || cell(newPlayerPos) == BOX_ON_FLOOR) {
			if(!isValid(newSecondPos)) {
				continue;
			}
			if(cell(newSecondPos) == WALL) {
				continue;
			}
			if(cell(newSecondPos) == BOX_ON_SLOT || cell(newSecondPos) == BOX_ON_FLOOR) {
				continue;
			}
		}

		if(cell(playerPos) == PLAYER_ON_SLOT) {
			cell(playerPos) = EMPTY_SLOT;
		} else if(cell(playerPos) == PLAYER_ON_FLOOR) {
			cell(playerPos) = FLOOR;
		}
		if(cell(newPlayerPos) == EMPTY_SLOT) {
			cell(newPlayerPos) = PLAYER_ON_SLOT;
		} else if(cell(newPlayerPos) == FLOOR) {
			cell(newPlayerPos) = PLAYER_ON_FLOOR;
		} else if(cell(newPlayerPos) == BOX_ON_FLOOR || cell(newPlayerPos) == BOX_ON_SLOT) {
			if(cell(newPlayerPos) == BOX_ON_SLOT) {
				cell(newPlayerPos) = PLAYER_ON_SLOT;
			} else if(cell(newPlayerPos) == BOX_ON_FLOOR) {
				cell(newPlayerPos) = PLAYER_ON_FLOOR;
			}
			if(cell(newSecondPos) == EMPTY_SLOT) {
				cell(newSecondPos) = BOX_ON_SLOT;
			} else if(cell(newSecondPos) == FLOOR) {
				cell(newSecondPos) = BOX_ON_FLOOR;
			}
			control = control.toUpper();
		}
		history.append(control);
	}
}

QString Sokoban::toString() const
{
	QMap<QChar, int> cellToChar;
	cellToChar[FLOOR          ] = ' ';
	cellToChar[WALL           ] = '#';
	cellToChar[PLAYER_ON_FLOOR] = '@';
	cellToChar[EMPTY_SLOT     ] = '.';
	cellToChar[PLAYER_ON_SLOT ] = '+';
	cellToChar[BOX_ON_FLOOR   ] = '$';
	cellToChar[BOX_ON_SLOT    ] = '*';

	QString result;
	for(int y = 0; y < size.height(); ++y) {
		for(int x = 0; x < size.width(); ++x) {
			result.append(cellToChar[cells[x + y * size.width()]]);
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
	return false;
}

bool Sokoban::isSolved()
{
	int freeBoxCount = 0, freeSlotCount = 0;
	for(int y = 0; y < size.height(); ++y) {
		for(int x = 0; x < size.width(); ++x) {
			switch(cell(QPoint(x, y))) {
				case EMPTY_SLOT: freeSlotCount++; break;
				case PLAYER_ON_SLOT: freeSlotCount++; break;
				case BOX_ON_FLOOR: freeBoxCount++; break;
			}
		}
	}
	return (freeBoxCount == 0) && (freeSlotCount == 0);
}

//! Use `qmake "CONFIG += SOKOBAN_TEST"` to build unit tests.
#ifdef SOKOBAN_TEST
#include <QtTest/QtTest>
class SokobanTest : public QObject {
	Q_OBJECT
private slots:
	void levelsAreStrings_data() {
		QTest::addColumn<QString>("level");
		QTest::addColumn<int>("width");
		QTest::addColumn<int>("height");
		QTest::newRow("1x1") << "@" << 1 << 1;
		QTest::newRow("3x1") << "@ ." << 3 << 1;
		QTest::newRow("1x3") << "@\n \n$" << 1 << 3;
		QTest::newRow("3x3") << "@  \n$$$\n###" << 3 << 3;
	}
	void levelsAreStrings() {
		QFETCH(QString, level);
		QFETCH(int, width);
		QFETCH(int, height);
		Sokoban sokoban(level);
		QCOMPARE(sokoban.width(), width);
		QCOMPARE(sokoban.height(), height);
		QCOMPARE(sokoban.toString(), level);
	}
	void canMoveOntoFloor_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("right") << QString("@ "  ) << QString('r') << QString(" @");
		QTest::newRow("left") << QString(" @"  ) << QString('l') << QString("@ ");
		QTest::newRow("down") << QString("@\n ") << QString('d') << QString(" \n@");
		QTest::newRow("up") << QString(" \n@") << QString('u') << QString("@\n ");
	}
	void canMoveOntoFloor() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void cannotMoveOntoWall_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("right") << QString("@#") << QString('r') << QString("@#");
		QTest::newRow("left") << QString("#@") << QString('l') << QString("#@");
		QTest::newRow("down") << QString("@\n#") << QString('d') << QString("@\n#");
		QTest::newRow("up") << QString("#\n@") << QString('u') << QString("#\n@");
	}
	void cannotMoveOntoWall() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void canMoveOntoSlot_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("right") << QString("@.") << QString('r') << QString(" +");
		QTest::newRow("left") << QString(".@") << QString('l') << QString("+ ");
		QTest::newRow("down") << QString("@\n.") << QString('d') << QString(" \n+");
		QTest::newRow("up") << QString(".\n@") << QString('u') << QString("+\n ");
	}
	void canMoveOntoSlot() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void canMoveFromSlot_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("toEmpty:right") << QString("+ ") << QString('r') << QString(".@");
		QTest::newRow("toEmpty:left") << QString(" +") << QString('l') << QString("@.");
		QTest::newRow("toEmpty:down") << QString("+\n ") << QString('d') << QString(".\n@");
		QTest::newRow("toEmpty:up") << QString(" \n+") << QString('u') << QString("@\n.");
		QTest::newRow("toSlot:right") << QString("+.") << QString('r') << QString(".+");
		QTest::newRow("toSlot:left") << QString(".+") << QString('l') << QString("+.");
		QTest::newRow("toSlot:down") << QString("+\n.") << QString('d') << QString(".\n+");
		QTest::newRow("toSlot:up") << QString(".\n+") << QString('u') << QString("+\n.");
	}
	void canMoveFromSlot() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void canMoveOntoFreeBoxOnFloor_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("toEmpty:right") << QString("@$ ") << QString('r') << QString(" @$");
		QTest::newRow("toEmpty:left") << QString(" $@") << QString('l') << QString("$@ ");
		QTest::newRow("toEmpty:down") << QString("@\n$\n ") << QString('d') << QString(" \n@\n$");
		QTest::newRow("toEmpty:up") << QString(" \n$\n@") << QString('u') << QString("$\n@\n ");
		QTest::newRow("toSlot:right") << QString("@$.") << QString('r') << QString(" @*");
		QTest::newRow("toSlot:left") << QString(".$@") << QString('l') << QString("*@ ");
		QTest::newRow("toSlot:down") << QString("@\n$\n.") << QString('d') << QString(" \n@\n*");
		QTest::newRow("toSlot:up") << QString(".\n$\n@") << QString('u') << QString("*\n@\n ");
	}
	void canMoveOntoFreeBoxOnFloor() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void canMoveOntoFreeBoxOnSlot_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("toEmpty:right") << QString("@* ") << QString('r') << QString(" +$");
		QTest::newRow("toEmpty:left") << QString(" *@") << QString('l') << QString("$+ ");
		QTest::newRow("toEmpty:down") << QString("@\n*\n ") << QString('d') << QString(" \n+\n$");
		QTest::newRow("toEmpty:up") << QString(" \n*\n@") << QString('u') << QString("$\n+\n ");
		QTest::newRow("toSlot:right") << QString("@*.") << QString('r') << QString(" +*");
		QTest::newRow("toSlot:left") << QString(".*@") << QString('l') << QString("*+ ");
		QTest::newRow("toSlot:down") << QString("@\n*\n.") << QString('d') << QString(" \n+\n*");
		QTest::newRow("toSlot:up") << QString(".\n*\n@") << QString('u') << QString("*\n+\n ");
	}
	void canMoveOntoFreeBoxOnSlot() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void cannotMoveTwoBoxesInRow_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("right") << QString("@$$ ") << QString('r') << QString("@$$ ");
		QTest::newRow("left") << QString(" $$@") << QString('l') << QString(" $$@");
		QTest::newRow("down") << QString("@\n$\n$\n ") << QString('d') << QString("@\n$\n$\n ");
		QTest::newRow("up") << QString(" \n$\n$\n@") << QString('u') << QString(" \n$\n$\n@");
	}
	void cannotMoveTwoBoxesInRow() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}
	void cannotMoveBoxIntoWall_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::newRow("right") << QString("@$# ") << QString('r') << QString("@$# ");
		QTest::newRow("left") << QString(" #$@") << QString('l') << QString(" #$@");
		QTest::newRow("down") << QString("@\n$\n#\n ") << QString('d') << QString("@\n$\n#\n ");
		QTest::newRow("up") << QString(" \n#\n$\n@") << QString('u') << QString(" \n#\n$\n@");
	}
	void cannotMoveBoxIntoWall() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
	}

	void historyIsTracked_data() {
		QTest::addColumn<QString>("levelBefore");
		QTest::addColumn<QString>("control");
		QTest::addColumn<QString>("levelAfter");
		QTest::addColumn<QString>("history");
		QTest::newRow("right") << QString("@ ") << QString("r") << QString(" @") << QString("r");
		QTest::newRow("left") << QString(" @") << QString("l") << QString("@ ") << QString("l");
		QTest::newRow("up") << QString(" \n@") << QString("u") << QString("@\n ") << QString("u");
		QTest::newRow("down") << QString("@\n ") << QString("d") << QString(" \n@") << QString("d");
		QTest::newRow("box right") << QString("@$.") << QString("r") << QString(" @*") << QString("R");
		QTest::newRow("box left") << QString(".$@") << QString("l") << QString("*@ ") << QString("L");
		QTest::newRow("box up") << QString(".\n$\n@") << QString("u") << QString("*\n@\n ") << QString("U");
		QTest::newRow("box down") << QString("@\n$\n.") << QString("d") << QString(" \n@\n*") << QString("D");
		QTest::newRow("into wall") << QString(" #@") << QString("l") << QString(" #@") << QString("");
		QTest::newRow("box into wall") << QString(" #$@") << QString("l") << QString(" #$@") << QString("");
		QTest::newRow("two boxes") << QString(" $$@") << QString("l") << QString(" $$@") << QString("");
		QTest::newRow("complex history") << QString("#.* @") << QString("lll") << QString("#*+  ") << QString("lL");
	}
	void historyIsTracked() {
		QFETCH(QString, levelBefore);
		QFETCH(QString, control);
		QFETCH(QString, levelAfter);
		QFETCH(QString, history);
		Sokoban sokoban(levelBefore);
		sokoban.processControls(control);
		QCOMPARE(sokoban.toString(), levelAfter);
		QCOMPARE(sokoban.historyAsString(), history);
	}
	void takeAllSlotsToWin_data() {
		QTest::addColumn<QString>("level");
		QTest::addColumn<bool>("isSolved");
		QTest::newRow("empty") << QString("@ ") << true;
		QTest::newRow("box without slot") << QString("@$") << false;
		QTest::newRow("solved box") << QString("@*") << true;
		QTest::newRow("unsolved box") << QString("+*") << false;
		QTest::newRow("two boxes, one slot") << QString("@*$ ") << false;
		QTest::newRow("two solved boxes") << QString("@* * ") << true;
		QTest::newRow("two boxes, three slots") << QString("@*.* ") << false;
		QTest::newRow("two boxes, four slots") << QString("+*.* ") << false;
		QTest::newRow("three boxes, four slots") << QString("+*.*$") << false;
	}
	void takeAllSlotsToWin() {
		QFETCH(QString, level);
		QFETCH(bool, isSolved);
		Sokoban sokoban(level);
		QCOMPARE(sokoban.isSolved(), isSolved);
	}
	void undoMovement() {
		QString levelBefore = ".* \n.$@";
		QString controls = "luruldu";
		QString history = "LruLdu";
		QString levelAfter = "*+ \n*  ";
		Sokoban sokoban(levelBefore);
		QCOMPARE(sokoban.toString(), levelBefore);
		sokoban.processControls(controls);
		QCOMPARE(sokoban.toString(), levelAfter);
		QCOMPARE(sokoban.historyAsString(), history);
		while(sokoban.undo()) {}
		QCOMPARE(sokoban.historyAsString(), QString());
		QCOMPARE(sokoban.toString(), levelBefore);
	}
	void exceptionInvalidPlayerCount_data() {
		QTest::addColumn<QString>("field");
		QTest::addColumn<bool>("shouldBeThrown");
		QTest::addColumn<int>("playerCount");
		QTest::newRow("3 players") << QString("@@@") << true << 3;
		QTest::newRow("2 players") << QString("@@ ") << true << 2;
		QTest::newRow("1 players") << QString("#@ ") << false << 1;
		QTest::newRow("0 players") << QString("#  ") << true << 0;
	}
	void exceptionInvalidPlayerCount() {
		QFETCH(QString, field);
		QFETCH(bool, shouldBeThrown);
		QFETCH(int, playerCount);
		bool thrown = false;
		try {
			Sokoban sokoban(field);
		} catch(Sokoban::InvalidPlayerCountException & e) {
			QCOMPARE(e.playerCount, playerCount);
			thrown = true;
		} catch(...) {}
		QCOMPARE(thrown, shouldBeThrown);
	}
	void exceptionInvalidControl_data() {
		QTest::addColumn<QChar>("control");
		QTest::addColumn<bool>("shouldBeThrown");
		QTest::newRow("r") << QChar('r') << false;
		QTest::newRow("l") << QChar('l') << false;
		QTest::newRow("u") << QChar('u') << false;
		QTest::newRow("d") << QChar('d') << false;
		QTest::newRow("R") << QChar('R') << true;
		QTest::newRow("L") << QChar('L') << true;
		QTest::newRow("U") << QChar('U') << true;
		QTest::newRow("D") << QChar('D') << true;
		QTest::newRow("t") << QChar('t') << true;
		QTest::newRow("_") << QChar('_') << true;
		QTest::newRow("0") << QChar('0') << true;
		QTest::newRow("1") << QChar('1') << true;
	}
	void exceptionInvalidControl() {
		QFETCH(QChar, control);
		QFETCH(bool, shouldBeThrown);
		QString field = "   \n @ \n   ";
		bool thrown = false;
		try {
			Sokoban sokoban(field);
			sokoban.processControls(control);
		} catch(Sokoban::InvalidControlException & e) {
			QCOMPARE(e.invalidControl, control);
			thrown = true;
		} catch(...) {}
		QCOMPARE(thrown, shouldBeThrown);
	}
	void exceptionInvalidUndo_data() {
		QTest::addColumn<QString>("field");
		QTest::addColumn<QString>("history");
		QTest::addColumn<bool>   ("shouldBeThrown");
		QTest::newRow("") << QString("@ ") << QString("") << false;
		QTest::newRow("space") << QString("@ ") << QString(" ") << true;
		QTest::newRow("l") << QString("@ ") << QString("l") << false;
		QTest::newRow("l#") << QString("@#") << QString("l") << true;
	}
	void exceptionInvalidUndo() {
		QFETCH(QString, field);
		QFETCH(QString, history);
		QFETCH(bool, shouldBeThrown);
		int historySize = history.size();
		bool thrown = false;
		Sokoban sokoban(field, history);
		try {
			sokoban.undo();
		} catch(Sokoban::InvalidUndoException & e) {
			QString history = sokoban.historyAsString();
			QCOMPARE(history.size(), historySize);
			QVERIFY(!history.isEmpty());
			QChar last = history[history.size() - 1];
			QCOMPARE(e.invalidUndoControl, last);
			thrown = true;
		} catch(...) {}
		QCOMPARE(thrown, shouldBeThrown);
	}
	void exceptionWhenMoveOutOfMap_data() {
		QTest::addColumn<QString>("field");
		QTest::addColumn<QChar>("control");
		QTest::addColumn<bool>("shouldBeThrown");
		QTest::newRow("u") << QString("@ ") << QChar('u') << true;
		QTest::newRow("d") << QString("@ ") << QChar('d') << true;
		QTest::newRow("l") << QString("@ ") << QChar('l') << true;
		QTest::newRow("r_") << QString("@ ") << QChar('r') << false;
		QTest::newRow("r") << QString(" @") << QChar('r') << true;
	}
	void exceptionWhenMoveOutOfMap() {
		QFETCH(QString, field);
		QFETCH(QChar, control);
		QFETCH(bool, shouldBeThrown);
		bool thrown = false;
		try {
			Sokoban sokoban(field);
			sokoban.processControls(control);
		} catch(Sokoban::OutOfMapException & e) {
			thrown = true;
		} catch(...) {}
		QCOMPARE(thrown, shouldBeThrown);
	}
};
QTEST_MAIN(SokobanTest)
#include "sokoban.moc"
#endif//SOKOBAN_TEST
