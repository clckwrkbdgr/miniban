//! Use `qmake "CONFIG += SOKOBAN_TEST"` to build unit tests.
#include "sokoban.h"
#include <chthon/test.h>
using namespace Chthon::UnitTest;

std::string to_string(const QPoint & point)
{
	return Chthon::format("({0}, {1})", point.x(), point.y());
}

std::string to_string(const QString & str)
{
	return str.toStdString();
}


TEST(levelsAreStrings)
{
	Sokoban sokoban1("@ $.#");
	EQUAL(sokoban1.width(), 5);
	EQUAL(sokoban1.height(), 1);
	EQUAL(sokoban1.getPlayerPos(), QPoint(0, 0));
	EQUAL(sokoban1.toString(), QString("@ $.#"));
	EQUAL(sokoban1.getCellAt(0, 0).type, int(Cell::FLOOR));
	ASSERT(sokoban1.getObjectAt(0, 0).is_player);
	EQUAL(sokoban1.getCellAt(1, 0).type, int(Cell::FLOOR));
	ASSERT(sokoban1.getObjectAt(1, 0).isNull());
	EQUAL(sokoban1.getCellAt(2, 0).type, int(Cell::FLOOR));
	ASSERT(!sokoban1.getObjectAt(2, 0).is_player);
	EQUAL(sokoban1.getCellAt(3, 0).type, int(Cell::SLOT));
	ASSERT(sokoban1.getObjectAt(3, 0).isNull());
	EQUAL(sokoban1.getCellAt(4, 0).type, int(Cell::WALL));
	ASSERT(sokoban1.getObjectAt(4, 0).isNull());

	Sokoban sokoban2("#.\n*+");
	EQUAL(sokoban2.width(), 2);
	EQUAL(sokoban2.height(), 2);
	EQUAL(sokoban2.getPlayerPos(), QPoint(1, 1));
	EQUAL(sokoban2.toString(), QString("#.\n*+"));
	EQUAL(sokoban2.getCellAt(0, 0).type, int(Cell::WALL));
	ASSERT(sokoban2.getObjectAt(0, 0).isNull());
	EQUAL(sokoban2.getCellAt(1, 0).type, int(Cell::SLOT));
	ASSERT(sokoban2.getObjectAt(1, 0).isNull());
	EQUAL(sokoban2.getCellAt(0, 1).type, int(Cell::SLOT));
	ASSERT(!sokoban2.getObjectAt(0, 1).is_player);
	EQUAL(sokoban2.getCellAt(1, 1).type, int(Cell::SLOT));
	ASSERT(sokoban2.getObjectAt(1, 1).is_player);
}

TEST(canMoveOntoFloor)
{
	Sokoban sokoban("@ ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString(" @"));
}

TEST(cannotMoveOntoWall)
{
	Sokoban sokoban("@#");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), QString("@#"));
}

TEST(canMoveOntoSlot)
{
	Sokoban sokoban("@.");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString(" +"));
}

TEST(canMoveFromSlot)
{
	Sokoban sokoban("+. ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString(".+ "));

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString("..@"));
}

TEST(canMoveOntoFreeBoxOnFloor)
{
	Sokoban sokoban("@$ .");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString(" @$."));

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString("  @*"));
}

TEST(canMoveOntoFreeBoxOnSlot)
{
	Sokoban sokoban("@*. ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString(" +* "));

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString(" .+$"));
}

TEST(cannotMoveTwoBoxesInRow)
{
	Sokoban sokoban("@$$ ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), QString("@$$ "));
}

TEST(cannotMoveBoxIntoWall)
{
	Sokoban sokoban("@$# ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), QString("@$# "));
}

TEST(canRunUntilWall)
{
	Sokoban sokoban("@  #");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString("  @#"));
}

TEST(cannotRunIntoWall)
{
	Sokoban sokoban("@# ");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), QString("@# "));
}

TEST(cannotPushBoxWhileRunning)
{
	Sokoban sokoban("@  $ #");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), QString("  @$ #"));
}

TEST(targetMoving)
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
	EQUAL(sokoban.getPlayerPos(), QPoint(5, 5));
	bool moved = false;

	moved = sokoban.movePlayer(QPoint(1, 1));
	ASSERT(!moved);
	EQUAL(sokoban.getPlayerPos(), QPoint(5, 5));
	EQUAL(sokoban.historyAsString(), QString());

	moved = sokoban.movePlayer(QPoint(6, 3));
	ASSERT(moved);
	EQUAL(sokoban.getPlayerPos(), QPoint(6, 3));
	EQUAL(sokoban.historyAsString(), QString("llddrrrruuuul"));

	moved = sokoban.movePlayer(QPoint(4, 3));
	ASSERT(moved);
	EQUAL(sokoban.getPlayerPos(), QPoint(4, 3));
	EQUAL(sokoban.historyAsString(), QString("llddrrrruuuulruullllddr"));

	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.getPlayerPos(), QPoint(6, 3));
	EQUAL(sokoban.historyAsString(), QString("llddrrrruuuulruullllddrRR"));

	moved = sokoban.movePlayer(QPoint(5, 5));
	ASSERT(!moved);
	EQUAL(sokoban.getPlayerPos(), QPoint(6, 3));
	EQUAL(sokoban.historyAsString(), QString("llddrrrruuuulruullllddrRR"));
}

TEST(should_move_diagonally_through_empty)
{
	Sokoban sokoban("@ \n  ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(1, 1));
}

TEST(should_move_diagonally_through_wall)
{
	Sokoban sokoban("@ \n# ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(1, 1));
}

TEST(should_move_diagonally_through_another_wall)
{
	Sokoban sokoban("@#\n  ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(1, 1));
}

TEST(should_move_diagonally_through_box)
{
	Sokoban sokoban("@ \n$ ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(1, 1));
}

TEST(should_move_diagonally_through_another_box)
{
	Sokoban sokoban("@$\n  ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(1, 1));
}

TEST(should_not_move_diagonally_through_closing_walls)
{
	Sokoban sokoban("@#\n# ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(0, 0));
}

TEST(should_not_move_diagonally_through_closing_boxes)
{
	Sokoban sokoban("@$\n$ ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(0, 0));
}

TEST(should_not_move_diagonally_through_wall_blocking)
{
	Sokoban sokoban("@ \n #");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(0, 0));
}

TEST(should_not_move_diagonally_through_box_blocking)
{
	Sokoban sokoban("@ \n $");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), QPoint(0, 0));
}

TEST(historyIsTracked)
{
	Sokoban sokoban("#@ $.$");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("# @$.$"));
	EQUAL(sokoban.historyAsString(), QString("r"));

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("#  @*$"));
	EQUAL(sokoban.historyAsString(), QString("rR"));

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("#  @*$"));
	EQUAL(sokoban.historyAsString(), QString("rR"));

	sokoban.runPlayer(Sokoban::LEFT);
	EQUAL(sokoban.toString(), QString("#@  *$"));
	EQUAL(sokoban.historyAsString(), QString("rRll"));
}

TEST(historyIsTrackedWithUndo)
{
	Sokoban sokoban("#@ $.$", "", true);

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("# @$.$"));
	EQUAL(sokoban.historyAsString(), QString("r"));

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("#  @*$"));
	EQUAL(sokoban.historyAsString(), QString("rR"));

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("#  @*$"));
	EQUAL(sokoban.historyAsString(), QString("rR"));

	sokoban.runPlayer(Sokoban::LEFT);
	EQUAL(sokoban.toString(), QString("#@  *$"));
	EQUAL(sokoban.historyAsString(), QString("rRll"));
	
	sokoban.undo();
	sokoban.undo();
	sokoban.undo();
	EQUAL(sokoban.toString(), QString("# @$.$"));
	EQUAL(sokoban.historyAsString(), QString("rRll---"));

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("#  @*$"));
	EQUAL(sokoban.historyAsString(), QString("rRll---R"));

	sokoban.undo();
	sokoban.undo();
	EQUAL(sokoban.toString(), QString("#@ $.$"));
	EQUAL(sokoban.historyAsString(), QString("rRll---R--"));

	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), QString("#  @*$"));
	EQUAL(sokoban.historyAsString(), QString("rRll---R--rR"));
}

TEST(should_win_when_empty)
{
	Sokoban sokoban("@ ");
	ASSERT(sokoban.isSolved());
}

TEST(should_not_win_when_box_without_slot)
{
	Sokoban sokoban("@$");
	ASSERT(!sokoban.isSolved());
}

TEST(should_win_when_solved_box)
{
	Sokoban sokoban("@*");
	ASSERT(sokoban.isSolved());
}

TEST(should_not_win_when_unsolved_box)
{
	Sokoban sokoban("+*");
	ASSERT(!sokoban.isSolved());
}

TEST(should_not_win_when_two_boxes_and_one_slot)
{
	Sokoban sokoban("@*$ ");
	ASSERT(!sokoban.isSolved());
}

TEST(should_win_when_two_solved_boxes)
{
	Sokoban sokoban("@* * ");
	ASSERT(sokoban.isSolved());
}

TEST(should_not_win_when_two_boxes_and_three_slots)
{
	Sokoban sokoban("@*.* ");
	ASSERT(!sokoban.isSolved());
}

TEST(should_not_win_when_two_boxes_and_four_slots)
{
	Sokoban sokoban("+*.* ");
	ASSERT(!sokoban.isSolved());
}

TEST(should_not_win_when_three_boxes_and_four_slots)
{
	Sokoban sokoban("+*.*$");
	ASSERT(!sokoban.isSolved());
}

TEST(undoMovement)
{
	Sokoban sokoban(".* \n.$@");
	sokoban.movePlayer(Sokoban::LEFT);
	sokoban.movePlayer(Sokoban::UP);
	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::UP);
	sokoban.movePlayer(Sokoban::LEFT);
	sokoban.movePlayer(Sokoban::DOWN);
	sokoban.movePlayer(Sokoban::UP);
	EQUAL(sokoban.toString(), QString("*+ \n*  "));
	EQUAL(sokoban.historyAsString(), QString("LruLdu"));

	while(sokoban.undo()) {}
	EQUAL(sokoban.historyAsString(), QString());
	EQUAL(sokoban.toString(), QString(".* \n.$@"));
}

TEST(should_consider_2_players_invalid)
{
	CATCH(Sokoban("@@ "), Sokoban::InvalidPlayerCountException, e) {
		EQUAL(e.playerCount, 2);
	}
}

TEST(should_consider_1_players_valid)
{
	Sokoban("#@ ");
}

TEST(should_consider_0_players_invalid)
{
	CATCH(Sokoban("#  "), Sokoban::InvalidPlayerCountException, e) {
		EQUAL(e.playerCount, 0);
	}
}

TEST(should_consider_empty_undo_string_valid)
{
	Sokoban sokoban("@ ", "");
	// Should not throw;
}

TEST(should_consider_valid_undo_string_valid)
{
	Sokoban sokoban("@ ", "l");
	// Should not throw;
}

TEST(should_consider_invalid_characters_undo_string_invalid)
{
	Sokoban sokoban("@ ", " ");
	CATCH(sokoban.undo(), Sokoban::InvalidUndoException, e) {
		EQUAL(sokoban.historyAsString().size(), 1);
		EQUAL(e.invalidUndoControl, ' ');
	}
}

TEST(should_consider_invalid_movement_undo_string_invalid)
{
	Sokoban sokoban("@#", "l");
	CATCH(sokoban.undo(), Sokoban::InvalidUndoException, e) {
		EQUAL(sokoban.historyAsString().size(), 1);
		EQUAL(e.invalidUndoControl, 'l');
	}
}

TEST(unreachableCellsAreMarkedAsSpace)
{
	QString level =
		"  ###  \n"
		"### ###\n"
		"#  @  #\n"
		"### ###\n"
		"  ###  ";
	Sokoban sokoban(level);
	EQUAL(sokoban.toString(), level);
	EQUAL(sokoban.getCellAt(QPoint(0, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(1, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(5, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(6, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(0, 4)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(1, 4)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(5, 4)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(QPoint(6, 4)).type, int(Cell::SPACE));
	int spaceCount = 0;
	for(int x = 0; x < sokoban.width(); ++x) {
		for(int y = 0; y < sokoban.height(); ++y) {
			if(sokoban.getCellAt(QPoint(x, y)).type == Cell::SPACE) {
				spaceCount++;
			}
		}
	}
	EQUAL(spaceCount, 8);
}

int main(int argc, char ** argv)
{
	run_all_tests(argc, argv);
}

