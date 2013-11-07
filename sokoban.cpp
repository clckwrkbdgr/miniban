#include <QtDebug>
#include <QtCore/QStringList>
#include "sokoban.h"

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
	charToCell[' '] = SPACE;
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

	cells.fill(Sokoban::SPACE, size.width() * size.height());
	int playerCount = 0;
	for(int y = 0; y < rows.count(); ++y) {
		const QString & row = rows[y];
		for(int x = 0; x < row.size(); ++x) {
			if(validChars.contains(row[x])) {
				cell(QPoint(x, y)) = charToCell[row[x]];
				if(cell(QPoint(x, y)) == PLAYER_ON_FLOOR || cell(QPoint(x, y)) == PLAYER_ON_SLOT) {
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
	if(cell(newPlayerPos) == BOX_ON_SLOT || cell(newPlayerPos) == BOX_ON_FLOOR) {
		if(cautious) {
			return false;
		}
		if(!isValid(newSecondPos)) {
			return false;
		}
		if(cell(newSecondPos) == WALL) {
			return false;
		}
		if(cell(newSecondPos) == BOX_ON_SLOT || cell(newSecondPos) == BOX_ON_FLOOR) {
			return false;
		}
	}

	QChar controlChar = charForControl[control];
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
	QMap<QChar, QPoint> shiftForControl;
	shiftForControl['u'] = QPoint(0, -1);
	shiftForControl['d'] = QPoint(0, 1);
	shiftForControl['r'] = QPoint(1, 0);
	shiftForControl['l'] = QPoint(-1, 0);
	if(history.isEmpty()) {
		return false;
	}

	QChar control = history.at(history.size() - 1);
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
	if(cell(oldPlayerPos) == BOX_ON_SLOT || cell(oldPlayerPos) == BOX_ON_FLOOR) {
		throw InvalidUndoException(control);
	}
	if(control.isUpper()) {
		if(!isValid(boxPos)) {
			throw InvalidUndoException(control);
		}
		if(cell(boxPos) != BOX_ON_SLOT && cell(boxPos) != BOX_ON_FLOOR) {
			throw InvalidUndoException(control);
		}
	}

	if(cell(oldPlayerPos) == EMPTY_SLOT) {
		cell(oldPlayerPos) = PLAYER_ON_SLOT;
	} else if(cell(oldPlayerPos) == FLOOR) {
		cell(oldPlayerPos) = PLAYER_ON_FLOOR;
	}
	if(control.isUpper()) {
		if(cell(playerPos) == PLAYER_ON_SLOT) {
			cell(playerPos) = BOX_ON_SLOT;
		} else if(cell(playerPos) == PLAYER_ON_FLOOR) {
			cell(playerPos) = BOX_ON_FLOOR;
		}
		if(cell(boxPos) == BOX_ON_SLOT) {
			cell(boxPos) = EMPTY_SLOT;
		} else if(cell(boxPos) == BOX_ON_FLOOR) {
			cell(boxPos) = FLOOR;
		}
	} else {
		if(cell(playerPos) == PLAYER_ON_SLOT) {
			cell(playerPos) = EMPTY_SLOT;
		} else if(cell(playerPos) == PLAYER_ON_FLOOR) {
			cell(playerPos) = FLOOR;
		}
	}
	history.remove(history.size() - 1, 1);
	return true;
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
		void levelsAreStrings() {
			Sokoban sokoban1("@ $.#");
			QCOMPARE(sokoban1.width(), 5);
			QCOMPARE(sokoban1.height(), 1);
			QCOMPARE(sokoban1.getPlayerPos(), QPoint(0, 0));
			QCOMPARE(sokoban1.toString(), QString("@ $.#"));
			QCOMPARE(sokoban1.getCell(0, 0), int(Sokoban::PLAYER_ON_FLOOR));
			QCOMPARE(sokoban1.getCell(1, 0), int(Sokoban::FLOOR));
			QCOMPARE(sokoban1.getCell(2, 0), int(Sokoban::BOX_ON_FLOOR));
			QCOMPARE(sokoban1.getCell(3, 0), int(Sokoban::EMPTY_SLOT));
			QCOMPARE(sokoban1.getCell(4, 0), int(Sokoban::WALL));

			Sokoban sokoban2("#.\n*+");
			QCOMPARE(sokoban2.width(), 2);
			QCOMPARE(sokoban2.height(), 2);
			QCOMPARE(sokoban2.getPlayerPos(), QPoint(1, 1));
			QCOMPARE(sokoban2.toString(), QString("#.\n*+"));
			QCOMPARE(sokoban2.getCell(0, 0), int(Sokoban::WALL));
			QCOMPARE(sokoban2.getCell(1, 0), int(Sokoban::EMPTY_SLOT));
			QCOMPARE(sokoban2.getCell(0, 1), int(Sokoban::BOX_ON_SLOT));
			QCOMPARE(sokoban2.getCell(1, 1), int(Sokoban::PLAYER_ON_SLOT));
		}
	void canMoveOntoFloor() {
		Sokoban sokoban("@ ");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString(" @"));
	}
	void cannotMoveOntoWall() {
		Sokoban sokoban("@#");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(!moved);
		QCOMPARE(sokoban.toString(), QString("@#"));
	}
	void canMoveOntoSlot() {
		Sokoban sokoban("@.");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString(" +"));
	}
	void canMoveFromSlot() {
		Sokoban sokoban("+. ");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString(".+ "));

		moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString("..@"));
	}
	void canMoveOntoFreeBoxOnFloor() {
		Sokoban sokoban("@$ .");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString(" @$."));

		moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString("  @*"));
	}
	void canMoveOntoFreeBoxOnSlot() {
		Sokoban sokoban("@*. ");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString(" +* "));

		moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString(" .+$"));
	}
	void cannotMoveTwoBoxesInRow() {
		Sokoban sokoban("@$$ ");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(!moved);
		QCOMPARE(sokoban.toString(), QString("@$$ "));
	}
	void cannotMoveBoxIntoWall() {
		Sokoban sokoban("@$# ");
		bool moved = sokoban.movePlayer(Sokoban::RIGHT);
		QVERIFY(!moved);
		QCOMPARE(sokoban.toString(), QString("@$# "));
	}
	void canRunUntilWall() {
		Sokoban sokoban("@  #");
		bool moved = sokoban.runPlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString("  @#"));
	}
	void cannotRunIntoWall() {
		Sokoban sokoban("@# ");
		bool moved = sokoban.runPlayer(Sokoban::RIGHT);
		QVERIFY(!moved);
		QCOMPARE(sokoban.toString(), QString("@# "));
	}
	void cannotPushBoxWhileRunning() {
		Sokoban sokoban("@  $ #");
		bool moved = sokoban.runPlayer(Sokoban::RIGHT);
		QVERIFY(moved);
		QCOMPARE(sokoban.toString(), QString("  @$ #"));
	}

	void historyIsTracked() {
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
	void exceptionWhenMoveOutOfMap() {
		try {
			Sokoban sokoban(" @");
			sokoban.movePlayer(Sokoban::RIGHT);
		} catch(Sokoban::OutOfMapException & e) {
			return;
		}
		QFAIL("Exception wasn't raised!");
	}

	void unreachableCellsAreMarkedAsSpace() {
		QString level =
			"  ###  \n"
			"### ###\n"
			"#  @  #\n"
			"### ###\n"
			"  ###  ";
		Sokoban sokoban(level);
		QCOMPARE(sokoban.toString(), level);
		QCOMPARE(sokoban.getCell(QPoint(0, 0)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(1, 0)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(5, 0)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(6, 0)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(0, 4)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(1, 4)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(5, 4)), int(Sokoban::SPACE));
		QCOMPARE(sokoban.getCell(QPoint(6, 4)), int(Sokoban::SPACE));
		int spaceCount = 0;
		for(int x = 0; x < sokoban.width(); ++x) {
			for(int y = 0; y < sokoban.height(); ++y) {
				if(sokoban.getCell(QPoint(x, y)) == Sokoban::SPACE) {
					spaceCount++;
				}
			}
		}
		QCOMPARE(spaceCount, 8);
	}
};
QTEST_MAIN(SokobanTest)
#include "sokoban.moc"
#endif//SOKOBAN_TEST
