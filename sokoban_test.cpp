//! Use `qmake "CONFIG += SOKOBAN_TEST"` to build unit tests.
#include "sokoban.h"
#include <QtTest/QtTest>
class SokobanTest : public QObject {
	Q_OBJECT
private slots:
	void levelsAreStrings();
	void canMoveOntoFloor();
	void cannotMoveOntoWall();
	void canMoveOntoSlot();
	void canMoveFromSlot();
	void canMoveOntoFreeBoxOnFloor();
	void canMoveOntoFreeBoxOnSlot();
	void cannotMoveTwoBoxesInRow();
	void cannotMoveBoxIntoWall();
	void canRunUntilWall();
	void cannotRunIntoWall();
	void cannotPushBoxWhileRunning();
	void targetMoving();
	void diagonalMovement_data();
	void diagonalMovement();
	void historyIsTracked();
	void historyIsTrackedWithUndo();
	void takeAllSlotsToWin_data();
	void takeAllSlotsToWin();
	void undoMovement();
	void exceptionInvalidPlayerCount_data();
	void exceptionInvalidPlayerCount();
	void exceptionInvalidUndo_data();
	void exceptionInvalidUndo();
	void unreachableCellsAreMarkedAsSpace();
};

void SokobanTest::levelsAreStrings()
{
	Sokoban sokoban1("@ $.#");
	QCOMPARE(sokoban1.width(), 5);
	QCOMPARE(sokoban1.height(), 1);
	QCOMPARE(sokoban1.getPlayerPos(), QPoint(0, 0));
	QCOMPARE(sokoban1.toString(), QString("@ $.#"));
	QCOMPARE(sokoban1.getCellAt(0, 0).type, int(Cell::FLOOR));
	QVERIFY(sokoban1.getObjectAt(0, 0).is_player);
	QCOMPARE(sokoban1.getCellAt(1, 0).type, int(Cell::FLOOR));
	QVERIFY(sokoban1.getObjectAt(1, 0).isNull());
	QCOMPARE(sokoban1.getCellAt(2, 0).type, int(Cell::FLOOR));
	QVERIFY(!sokoban1.getObjectAt(2, 0).is_player);
	QCOMPARE(sokoban1.getCellAt(3, 0).type, int(Cell::SLOT));
	QVERIFY(sokoban1.getObjectAt(3, 0).isNull());
	QCOMPARE(sokoban1.getCellAt(4, 0).type, int(Cell::WALL));
	QVERIFY(sokoban1.getObjectAt(4, 0).isNull());

	Sokoban sokoban2("#.\n*+");
	QCOMPARE(sokoban2.width(), 2);
	QCOMPARE(sokoban2.height(), 2);
	QCOMPARE(sokoban2.getPlayerPos(), QPoint(1, 1));
	QCOMPARE(sokoban2.toString(), QString("#.\n*+"));
	QCOMPARE(sokoban2.getCellAt(0, 0).type, int(Cell::WALL));
	QVERIFY(sokoban2.getObjectAt(0, 0).isNull());
	QCOMPARE(sokoban2.getCellAt(1, 0).type, int(Cell::SLOT));
	QVERIFY(sokoban2.getObjectAt(1, 0).isNull());
	QCOMPARE(sokoban2.getCellAt(0, 1).type, int(Cell::SLOT));
	QVERIFY(!sokoban2.getObjectAt(0, 1).is_player);
	QCOMPARE(sokoban2.getCellAt(1, 1).type, int(Cell::SLOT));
	QVERIFY(sokoban2.getObjectAt(1, 1).is_player);
}

void SokobanTest::canMoveOntoFloor()
{
	Sokoban sokoban("@ ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString(" @"));
}

void SokobanTest::cannotMoveOntoWall()
{
	Sokoban sokoban("@#");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(!moved);
	QCOMPARE(sokoban.toString(), QString("@#"));
}

void SokobanTest::canMoveOntoSlot()
{
	Sokoban sokoban("@.");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString(" +"));
}

void SokobanTest::canMoveFromSlot()
{
	Sokoban sokoban("+. ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString(".+ "));

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString("..@"));
}

void SokobanTest::canMoveOntoFreeBoxOnFloor()
{
	Sokoban sokoban("@$ .");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString(" @$."));

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString("  @*"));
}

void SokobanTest::canMoveOntoFreeBoxOnSlot()
{
	Sokoban sokoban("@*. ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString(" +* "));

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString(" .+$"));
}

void SokobanTest::cannotMoveTwoBoxesInRow()
{
	Sokoban sokoban("@$$ ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(!moved);
	QCOMPARE(sokoban.toString(), QString("@$$ "));
}

void SokobanTest::cannotMoveBoxIntoWall()
{
	Sokoban sokoban("@$# ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	QVERIFY(!moved);
	QCOMPARE(sokoban.toString(), QString("@$# "));
}

void SokobanTest::canRunUntilWall()
{
	Sokoban sokoban("@  #");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString("  @#"));
}

void SokobanTest::cannotRunIntoWall()
{
	Sokoban sokoban("@# ");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	QVERIFY(!moved);
	QCOMPARE(sokoban.toString(), QString("@# "));
}

void SokobanTest::cannotPushBoxWhileRunning()
{
	Sokoban sokoban("@  $ #");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	QVERIFY(moved);
	QCOMPARE(sokoban.toString(), QString("  @$ #"));
}

void SokobanTest::targetMoving()
{
	Sokoban sokoban(
		"#########\n"
		"# #     #\n"
		"# # ### #\n"
		"# #  $  #\n"
		"# ##### #\n"
		"# #  @# #\n"
		"# # ### #\n"
		"# #     #\n"
		"#########\n"
		);
	QCOMPARE(sokoban.getPlayerPos(), QPoint(5, 5));
	bool moved = false;

	moved = sokoban.movePlayer(QPoint(1, 1));
	QVERIFY(!moved);
	QCOMPARE(sokoban.getPlayerPos(), QPoint(5, 5));
	QCOMPARE(sokoban.historyAsString(), QString());

	moved = sokoban.movePlayer(QPoint(6, 3));
	QVERIFY(moved);
	QCOMPARE(sokoban.getPlayerPos(), QPoint(6, 3));
	QCOMPARE(sokoban.historyAsString(), QString("llddrrrruuuul"));

	moved = sokoban.movePlayer(QPoint(4, 3));
	QVERIFY(moved);
	QCOMPARE(sokoban.getPlayerPos(), QPoint(4, 3));
	QCOMPARE(sokoban.historyAsString(), QString("llddrrrruuuulruullllddr"));

	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.getPlayerPos(), QPoint(6, 3));
	QCOMPARE(sokoban.historyAsString(), QString("llddrrrruuuulruullllddrRR"));

	moved = sokoban.movePlayer(QPoint(5, 5));
	QVERIFY(!moved);
	QCOMPARE(sokoban.getPlayerPos(), QPoint(6, 3));
	QCOMPARE(sokoban.historyAsString(), QString("llddrrrruuuulruullllddrRR"));
}

void SokobanTest::diagonalMovement_data()
{
	QTest::addColumn<QString>("level");
	QTest::addColumn<int>("control");
	QTest::addColumn<bool>("moved");
	QTest::addColumn<QPoint>("new_pos");
	QTest::newRow("empty")         << QString("@ \n  ") << int(Sokoban::DOWN_RIGHT) << true << QPoint(1, 1);
	QTest::newRow("wall")          << QString("@ \n# ") << int(Sokoban::DOWN_RIGHT) << true << QPoint(1, 1);
	QTest::newRow("another wall")  << QString("@#\n  ") << int(Sokoban::DOWN_RIGHT) << true << QPoint(1, 1);
	QTest::newRow("box")           << QString("@ \n$ ") << int(Sokoban::DOWN_RIGHT) << true << QPoint(1, 1);
	QTest::newRow("another box")   << QString("@$\n  ") << int(Sokoban::DOWN_RIGHT) << true << QPoint(1, 1);
	QTest::newRow("closing walls") << QString("@#\n# ") << int(Sokoban::DOWN_RIGHT) << false << QPoint(0, 0);
	QTest::newRow("closing boxes") << QString("@$\n$ ") << int(Sokoban::DOWN_RIGHT) << false << QPoint(0, 0);
	QTest::newRow("wall blocking") << QString("@ \n #") << int(Sokoban::DOWN_RIGHT) << false << QPoint(0, 0);
	QTest::newRow("box blocking")  << QString("@ \n $") << int(Sokoban::DOWN_RIGHT) << false << QPoint(0, 0);
}
void SokobanTest::diagonalMovement()
{
	QFETCH(QString, level);
	QFETCH(int, control);
	QFETCH(bool, moved);
	QFETCH(QPoint, new_pos);
	Sokoban sokoban(level);
	bool ok = sokoban.movePlayer(control);
	QCOMPARE(ok, moved);
	QCOMPARE(sokoban.getPlayerPos(), new_pos);
}

void SokobanTest::historyIsTracked()
{
	Sokoban sokoban("#@ $.$");

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("# @$.$"));
	QCOMPARE(sokoban.historyAsString(), QString("r"));

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("#  @*$"));
	QCOMPARE(sokoban.historyAsString(), QString("rR"));

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("#  @*$"));
	QCOMPARE(sokoban.historyAsString(), QString("rR"));

	sokoban.runPlayer(Sokoban::LEFT);
	QCOMPARE(sokoban.toString(), QString("#@  *$"));
	QCOMPARE(sokoban.historyAsString(), QString("rRll"));
}

void SokobanTest::historyIsTrackedWithUndo()
{
	Sokoban sokoban("#@ $.$", "", true);

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("# @$.$"));
	QCOMPARE(sokoban.historyAsString(), QString("r"));

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("#  @*$"));
	QCOMPARE(sokoban.historyAsString(), QString("rR"));

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("#  @*$"));
	QCOMPARE(sokoban.historyAsString(), QString("rR"));

	sokoban.runPlayer(Sokoban::LEFT);
	QCOMPARE(sokoban.toString(), QString("#@  *$"));
	QCOMPARE(sokoban.historyAsString(), QString("rRll"));
	
	sokoban.undo();
	sokoban.undo();
	sokoban.undo();
	QCOMPARE(sokoban.toString(), QString("# @$.$"));
	QCOMPARE(sokoban.historyAsString(), QString("rRll---"));

	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("#  @*$"));
	QCOMPARE(sokoban.historyAsString(), QString("rRll---R"));

	sokoban.undo();
	sokoban.undo();
	QCOMPARE(sokoban.toString(), QString("#@ $.$"));
	QCOMPARE(sokoban.historyAsString(), QString("rRll---R--"));

	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::RIGHT);
	QCOMPARE(sokoban.toString(), QString("#  @*$"));
	QCOMPARE(sokoban.historyAsString(), QString("rRll---R--rR"));
}

void SokobanTest::takeAllSlotsToWin_data()
{
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
void SokobanTest::takeAllSlotsToWin()
{
	QFETCH(QString, level);
	QFETCH(bool, isSolved);
	Sokoban sokoban(level);
	QCOMPARE(sokoban.isSolved(), isSolved);
}

void SokobanTest::undoMovement()
{
	Sokoban sokoban(".* \n.$@");
	sokoban.movePlayer(Sokoban::LEFT);
	sokoban.movePlayer(Sokoban::UP);
	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::UP);
	sokoban.movePlayer(Sokoban::LEFT);
	sokoban.movePlayer(Sokoban::DOWN);
	sokoban.movePlayer(Sokoban::UP);
	QCOMPARE(sokoban.toString(), QString("*+ \n*  "));
	QCOMPARE(sokoban.historyAsString(), QString("LruLdu"));

	while(sokoban.undo()) {}
	QCOMPARE(sokoban.historyAsString(), QString());
	QCOMPARE(sokoban.toString(), QString(".* \n.$@"));
}

void SokobanTest::exceptionInvalidPlayerCount_data()
{
	QTest::addColumn<QString>("field");
	QTest::addColumn<bool>("shouldBeThrown");
	QTest::addColumn<int>("playerCount");
	QTest::newRow("3 players") << QString("@@@") << true << 3;
	QTest::newRow("2 players") << QString("@@ ") << true << 2;
	QTest::newRow("1 players") << QString("#@ ") << false << 1;
	QTest::newRow("0 players") << QString("#  ") << true << 0;
}
void SokobanTest::exceptionInvalidPlayerCount()
{
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

void SokobanTest::exceptionInvalidUndo_data()
{
	QTest::addColumn<QString>("field");
	QTest::addColumn<QString>("history");
	QTest::addColumn<bool>   ("shouldBeThrown");
	QTest::newRow("") << QString("@ ") << QString("") << false;
	QTest::newRow("space") << QString("@ ") << QString(" ") << true;
	QTest::newRow("l") << QString("@ ") << QString("l") << false;
	QTest::newRow("l#") << QString("@#") << QString("l") << true;
}
void SokobanTest::exceptionInvalidUndo()
{
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

void SokobanTest::unreachableCellsAreMarkedAsSpace()
{
	QString level =
		"  ###  \n"
		"### ###\n"
		"#  @  #\n"
		"### ###\n"
		"  ###  ";
	Sokoban sokoban(level);
	QCOMPARE(sokoban.toString(), level);
	QCOMPARE(sokoban.getCellAt(QPoint(0, 0)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(1, 0)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(5, 0)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(6, 0)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(0, 4)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(1, 4)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(5, 4)).type, int(Cell::SPACE));
	QCOMPARE(sokoban.getCellAt(QPoint(6, 4)).type, int(Cell::SPACE));
	int spaceCount = 0;
	for(int x = 0; x < sokoban.width(); ++x) {
		for(int y = 0; y < sokoban.height(); ++y) {
			if(sokoban.getCellAt(QPoint(x, y)).type == Cell::SPACE) {
				spaceCount++;
			}
		}
	}
	QCOMPARE(spaceCount, 8);
}

QTEST_MAIN(SokobanTest)
#include "sokoban_test.moc"

