#include <QtDebug>
#include <QtCore/QStringList>
#include "sokoban.h"

//# Miniban - a clone of Sokoban game.
//#
//# Developed using C++/Qt4.
//#
//# THE GAME
//#

namespace Sokoban { // Auxiliary functions.

using namespace TileType;
using namespace Control;

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

	//# Player can move in four directions (up, down, left, right).
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

bool isFreeToMove(const QString & field, int pos)
{
	//# Player cannot move through walls.
	return field[pos] != WALL;
}

};

namespace Sokoban { // Main functions.

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
	int freeSlotCount = field.count(EMPTY_SLOT) + field.count(PLAYER_ON_SLOT);
	int freeBoxCount =  field.count(BOX_ON_FLOOR);
	return (freeSlotCount == 0) && (freeBoxCount == 0);
}

};

//! Use `qmake "CONFIG += SOKOBAN_TEST"` to build unit tests.
#ifdef SOKOBAN_TEST
#include <QtTest/QtTest>
class SokobanTest : public QObject {
	Q_OBJECT
private slots:
	void canMoveOntoFloor() {
		QString a, b;
		a = Sokoban::process("@ ", 'r');   b = " @";   QCOMPARE(a, b);
		a = Sokoban::process(" @", 'l');   b = "@ ";   QCOMPARE(a, b);
		a = Sokoban::process("@\n ", 'd'); b = " \n@"; QCOMPARE(a, b);
		a = Sokoban::process(" \n@", 'u'); b = "@\n "; QCOMPARE(a, b);
	}
	void cannotMoveOntoWall() {
		QString a, b;
		a = Sokoban::process("@#", 'r');   b = "@#";   QCOMPARE(a, b);
		a = Sokoban::process("#@", 'l');   b = "#@";   QCOMPARE(a, b);
		a = Sokoban::process("@\n#", 'd'); b = "@\n#"; QCOMPARE(a, b);
		a = Sokoban::process("#\n@", 'u'); b = "#\n@"; QCOMPARE(a, b);
	}
	void canMoveOntoSlot() {
		QString a, b;
		a = Sokoban::process("@.", 'r');   b = " +";   QCOMPARE(a, b);
		a = Sokoban::process(".@", 'l');   b = "+ ";   QCOMPARE(a, b);
		a = Sokoban::process("@\n.", 'd'); b = " \n+"; QCOMPARE(a, b);
		a = Sokoban::process(".\n@", 'u'); b = "+\n "; QCOMPARE(a, b);
	}
	void canMoveFromSlot() {
		QString a, b;
		a = Sokoban::process("+ ", 'r');   b = ".@";   QCOMPARE(a, b);
		a = Sokoban::process(" +", 'l');   b = "@.";   QCOMPARE(a, b);
		a = Sokoban::process("+\n ", 'd'); b = ".\n@"; QCOMPARE(a, b);
		a = Sokoban::process(" \n+", 'u'); b = "@\n."; QCOMPARE(a, b);

		a = Sokoban::process("+.", 'r');   b = ".+";   QCOMPARE(a, b);
		a = Sokoban::process(".+", 'l');   b = "+.";   QCOMPARE(a, b);
		a = Sokoban::process("+\n.", 'd'); b = ".\n+"; QCOMPARE(a, b);
		a = Sokoban::process(".\n+", 'u'); b = "+\n."; QCOMPARE(a, b);
	}
	void canMoveOntoFreeBoxOnFloor() {
		QString a, b;
		a = Sokoban::process("@$ ", 'r');     b = " @$";     QCOMPARE(a, b);
		a = Sokoban::process(" $@", 'l');     b = "$@ ";     QCOMPARE(a, b);
		a = Sokoban::process("@\n$\n ", 'd'); b = " \n@\n$"; QCOMPARE(a, b);
		a = Sokoban::process(" \n$\n@", 'u'); b = "$\n@\n "; QCOMPARE(a, b);

		a = Sokoban::process("@$.", 'r');     b = " @*";     QCOMPARE(a, b);
		a = Sokoban::process(".$@", 'l');     b = "*@ ";     QCOMPARE(a, b);
		a = Sokoban::process("@\n$\n.", 'd'); b = " \n@\n*"; QCOMPARE(a, b);
		a = Sokoban::process(".\n$\n@", 'u'); b = "*\n@\n "; QCOMPARE(a, b);
	}
	void canMoveOntoFreeBoxOnSlot() {
		QString a, b;
		a = Sokoban::process("@* ", 'r');     b = " +$";     QCOMPARE(a, b);
		a = Sokoban::process(" *@", 'l');     b = "$+ ";     QCOMPARE(a, b);
		a = Sokoban::process("@\n*\n ", 'd'); b = " \n+\n$"; QCOMPARE(a, b);
		a = Sokoban::process(" \n*\n@", 'u'); b = "$\n+\n "; QCOMPARE(a, b);

		a = Sokoban::process("@*.", 'r');     b = " +*";     QCOMPARE(a, b);
		a = Sokoban::process(".*@", 'l');     b = "*+ ";     QCOMPARE(a, b);
		a = Sokoban::process("@\n*\n.", 'd'); b = " \n+\n*"; QCOMPARE(a, b);
		a = Sokoban::process(".\n*\n@", 'u'); b = "*\n+\n "; QCOMPARE(a, b);
	}
	void historyIsTracked() {
		QString a, b;
		QString history;
		a = Sokoban::process("#.@",     'l', NULL);     b = "#+ ";     QCOMPARE(a, b); QVERIFY(history.isEmpty());
		a = Sokoban::process("#.@",     'l', &history); b = "#+ ";     QCOMPARE(a, b); QCOMPARE(history, QString("l"));
		a = Sokoban::process("#+ ",     'l', &history); b = "#+ ";     QCOMPARE(a, b); QCOMPARE(history, QString("l"));
		a = Sokoban::process(".$@",     'l', &history); b = "*@ ";     QCOMPARE(a, b); QCOMPARE(history, QString("lL"));
		a = Sokoban::process(" $$@",    'l', &history); b = " $$@";    QCOMPARE(a, b); QCOMPARE(history, QString("lL"));
		a = Sokoban::process("$.*@",    'l', &history); b = "$*+ ";    QCOMPARE(a, b); QCOMPARE(history, QString("lLL"));
		a = Sokoban::process("@$.",     'r', &history); b = " @*";     QCOMPARE(a, b); QCOMPARE(history, QString("lLLR"));
		a = Sokoban::process(".\n$\n@", 'u', &history); b = "*\n@\n "; QCOMPARE(a, b); QCOMPARE(history, QString("lLLRU"));
		a = Sokoban::process("@\n$\n.", 'd', &history); b = " \n@\n*"; QCOMPARE(a, b); QCOMPARE(history, QString("lLLRUD"));
	}
	void cannotMoveTwoBoxesInRow() {
		QString a, b;
		a = Sokoban::process("@$$ ", 'r');       b = "@$$ ";       QCOMPARE(a, b);
		a = Sokoban::process(" $$@", 'l');       b = " $$@";       QCOMPARE(a, b);
		a = Sokoban::process("@\n$\n$\n ", 'd'); b = "@\n$\n$\n "; QCOMPARE(a, b);
		a = Sokoban::process(" \n$\n$\n@", 'u'); b = " \n$\n$\n@"; QCOMPARE(a, b);
	}
	void cannotMoveBoxIntoWall() {
		QString a, b;
		a = Sokoban::process("@$# ", 'r');       b = "@$# ";       QCOMPARE(a, b);
		a = Sokoban::process(" #$@", 'l');       b = " #$@";       QCOMPARE(a, b);
		a = Sokoban::process("@\n$\n#\n ", 'd'); b = "@\n$\n#\n "; QCOMPARE(a, b);
		a = Sokoban::process(" \n#\n$\n@", 'u'); b = " \n#\n$\n@"; QCOMPARE(a, b);
	}
	void takeAllSlotsToWin() {
		QVERIFY( Sokoban::isSolved("@ "));
		QVERIFY(!Sokoban::isSolved("@$"));
		QVERIFY( Sokoban::isSolved("@*"));
		QVERIFY(!Sokoban::isSolved("+*"));
		QVERIFY(!Sokoban::isSolved("@*$ "));
		QVERIFY( Sokoban::isSolved("@* * "));
		QVERIFY(!Sokoban::isSolved("@*.* "));
		QVERIFY(!Sokoban::isSolved("+*.* "));
		QVERIFY(!Sokoban::isSolved("+*.*$"));
	}
	void undoMovement() {
		QString history = "uldlrlrr";
		int historySize = history.size();
		QString a, b;
		a = Sokoban::undo("#. \n. @", &history); b = "#. \n.@ "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#. \n.@ ", &history); b = "#. \n+  "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#. \n+  ", &history); b = "#. \n.@ "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#. \n.@ ", &history); b = "#. \n+  "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#. \n+  ", &history); b = "#. \n.@ "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#. \n.@ ", &history); b = "#+ \n.  "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#+ \n.  ", &history); b = "#.@\n.  "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("#.@\n.  ", &history); b = "#. \n. @"; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
	}
	void undoMovementWithPush() {
		QString history = "UDLR";
		int historySize = history.size();
		QString a, b;
		a = Sokoban::undo(" @$", &history); b = "@$ "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("$@ ", &history); b = " $@"; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo(" \n@\n$", &history); b = "@\n$\n "; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
		a = Sokoban::undo("$\n@\n ", &history); b = " \n$\n@"; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
	}
	void exceptionInvalidPlayerCount_data() {
		QTest::addColumn<QString>("field");
		QTest::addColumn<QChar>("control");
		QTest::addColumn<bool>("shouldBeThrown");
		QTest::addColumn<int>("playerCount");
		QTest::newRow("3 players") << QString("@@@") << QChar('r') << true << 3;
		QTest::newRow("2 players") << QString("@@ ") << QChar('r') << true << 2;
		QTest::newRow("1 players") << QString("#@ ") << QChar('r') << false << 1;
		QTest::newRow("0 players") << QString("#  ") << QChar('r') << true << 0;
	}
	void exceptionInvalidPlayerCount() {
		QFETCH(QString, field);
		QFETCH(QChar, control);
		QFETCH(bool, shouldBeThrown);
		QFETCH(int, playerCount);
		bool thrown = false;
		try {
			Sokoban::process(field, control);
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
			Sokoban::process(field, control);
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
		try {
			Sokoban::undo(field, &history);
		} catch(Sokoban::InvalidUndoException & e) {
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
			Sokoban::process(field, control);
		} catch(Sokoban::OutOfMapException & e) {
			thrown = true;
		} catch(...) {}
		QCOMPARE(thrown, shouldBeThrown);
	}
};
QTEST_MAIN(SokobanTest)
#include "test.moc"
#endif//SOKOBAN_TEST
