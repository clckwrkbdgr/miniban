#include <QtTest/QtTest>
#include "sokoban.h"

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
		a = Sokoban::undo("#.@\n.  ", &history); b = "#  \n. @"; QCOMPARE(a, b); --historySize; QCOMPARE(history.size(), historySize);
	}
	void undoMovementWithPush() {
		QString history = "UDRL";
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
		QTest::newRow("r") << QChar('r') << true;
		QTest::newRow("l") << QChar('l') << true;
		QTest::newRow("u") << QChar('u') << true;
		QTest::newRow("d") << QChar('d') << true;
		QTest::newRow("R") << QChar('R') << false;
		QTest::newRow("L") << QChar('L') << false;
		QTest::newRow("U") << QChar('U') << false;
		QTest::newRow("D") << QChar('D') << false;
		QTest::newRow("t") << QChar('t') << false;
		QTest::newRow("_") << QChar('_') << false;
		QTest::newRow("0") << QChar('0') << false;
		QTest::newRow("1") << QChar('1') << false;
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
