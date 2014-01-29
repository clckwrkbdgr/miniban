#include "../src/sokoban.h"
#include <chthon/test.h>
#include <chthon/format.h>
using namespace Chthon::UnitTest;
using namespace Chthon;

SUITE(sokoban) {

TEST(levelsAreStrings)
{
	Sokoban sokoban1("@ $.#");
	EQUAL(sokoban1.width(), 5);
	EQUAL(sokoban1.height(), 1);
	EQUAL(sokoban1.getPlayerPos(), Chthon::Point(0, 0));
	EQUAL(sokoban1.toString(), "@ $.#");
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
	EQUAL(sokoban2.getPlayerPos(), Chthon::Point(1, 1));
	EQUAL(sokoban2.toString(), "#.\n*+");
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
	EQUAL(sokoban.toString(), " @");
}

TEST(cannotMoveOntoWall)
{
	Sokoban sokoban("@#");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), "@#");
}

TEST(canMoveOntoSlot)
{
	Sokoban sokoban("@.");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), " +");
}

TEST(canMoveFromSlot)
{
	Sokoban sokoban("+. ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), ".+ ");

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), "..@");
}

TEST(canMoveOntoFreeBoxOnFloor)
{
	Sokoban sokoban("@$ .");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), " @$.");

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), "  @*");
}

TEST(canMoveOntoFreeBoxOnSlot)
{
	Sokoban sokoban("@*. ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), " +* ");

	moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), " .+$");
}

TEST(cannotMoveTwoBoxesInRow)
{
	Sokoban sokoban("@$$ ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), "@$$ ");
}

TEST(cannotMoveBoxIntoWall)
{
	Sokoban sokoban("@$# ");
	bool moved = sokoban.movePlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), "@$# ");
}

TEST(canRunUntilWall)
{
	Sokoban sokoban("@  #");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), "  @#");
}

TEST(cannotRunIntoWall)
{
	Sokoban sokoban("@# ");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	ASSERT(!moved);
	EQUAL(sokoban.toString(), "@# ");
}

TEST(cannotPushBoxWhileRunning)
{
	Sokoban sokoban("@  $ #");
	bool moved = sokoban.runPlayer(Sokoban::RIGHT);
	ASSERT(moved);
	EQUAL(sokoban.toString(), "  @$ #");
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
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(5, 5));
	bool moved = false;

	moved = sokoban.movePlayer(Chthon::Point(1, 1));
	ASSERT(!moved);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(5, 5));
	EQUAL(sokoban.historyAsString(), "");

	moved = sokoban.movePlayer(Chthon::Point(6, 3));
	ASSERT(moved);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(6, 3));
	EQUAL(sokoban.historyAsString(), "llddrrrruuuul");

	moved = sokoban.movePlayer(Chthon::Point(4, 3));
	ASSERT(moved);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(4, 3));
	EQUAL(sokoban.historyAsString(), "llddrrrruuuulruullllddr");

	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(6, 3));
	EQUAL(sokoban.historyAsString(), "llddrrrruuuulruullllddrRR");

	moved = sokoban.movePlayer(Chthon::Point(5, 5));
	ASSERT(!moved);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(6, 3));
	EQUAL(sokoban.historyAsString(), "llddrrrruuuulruullllddrRR");
}

TEST(should_move_diagonally_through_empty)
{
	Sokoban sokoban("@ \n  ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(1, 1));
}

TEST(should_move_diagonally_through_wall)
{
	Sokoban sokoban("@ \n# ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(1, 1));
}

TEST(should_move_diagonally_through_another_wall)
{
	Sokoban sokoban("@#\n  ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(1, 1));
}

TEST(should_move_diagonally_through_box)
{
	Sokoban sokoban("@ \n$ ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(1, 1));
}

TEST(should_move_diagonally_through_another_box)
{
	Sokoban sokoban("@$\n  ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(1, 1));
}

TEST(should_not_move_diagonally_through_closing_walls)
{
	Sokoban sokoban("@#\n# ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(0, 0));
}

TEST(should_not_move_diagonally_through_closing_boxes)
{
	Sokoban sokoban("@$\n$ ");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(0, 0));
}

TEST(should_not_move_diagonally_through_wall_blocking)
{
	Sokoban sokoban("@ \n #");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(0, 0));
}

TEST(should_not_move_diagonally_through_box_blocking)
{
	Sokoban sokoban("@ \n $");
	bool ok = sokoban.movePlayer(Sokoban::DOWN_RIGHT);
	ASSERT(!ok);
	EQUAL(sokoban.getPlayerPos(), Chthon::Point(0, 0));
}

TEST(historyIsTracked)
{
	Sokoban sokoban("#@ $.$");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "# @$.$");
	EQUAL(sokoban.historyAsString(), "r");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "#  @*$");
	EQUAL(sokoban.historyAsString(), "rR");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "#  @*$");
	EQUAL(sokoban.historyAsString(), "rR");

	sokoban.runPlayer(Sokoban::LEFT);
	EQUAL(sokoban.toString(), "#@  *$");
	EQUAL(sokoban.historyAsString(), "rRll");
}

TEST(historyIsTrackedWithUndo)
{
	Sokoban sokoban("#@ $.$", "", true);

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "# @$.$");
	EQUAL(sokoban.historyAsString(), "r");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "#  @*$");
	EQUAL(sokoban.historyAsString(), "rR");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "#  @*$");
	EQUAL(sokoban.historyAsString(), "rR");

	sokoban.runPlayer(Sokoban::LEFT);
	EQUAL(sokoban.toString(), "#@  *$");
	EQUAL(sokoban.historyAsString(), "rRll");
	
	sokoban.undo();
	sokoban.undo();
	sokoban.undo();
	EQUAL(sokoban.toString(), "# @$.$");
	EQUAL(sokoban.historyAsString(), "rRll---");

	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "#  @*$");
	EQUAL(sokoban.historyAsString(), "rRll---R");

	sokoban.undo();
	sokoban.undo();
	EQUAL(sokoban.toString(), "#@ $.$");
	EQUAL(sokoban.historyAsString(), "rRll---R--");

	sokoban.movePlayer(Sokoban::RIGHT);
	sokoban.movePlayer(Sokoban::RIGHT);
	EQUAL(sokoban.toString(), "#  @*$");
	EQUAL(sokoban.historyAsString(), "rRll---R--rR");
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
	EQUAL(sokoban.toString(), "*+ \n*  ");
	EQUAL(sokoban.historyAsString(), "LruLdu");

	int count = 100;
	while(count --> 0) {
		sokoban.undo();
	}
	EQUAL(sokoban.historyAsString(), "");
	EQUAL(sokoban.toString(), ".* \n.$@");
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
	std::string level =
		"  ###  \n"
		"### ###\n"
		"#  @  #\n"
		"### ###\n"
		"  ###  ";
	Sokoban sokoban(level);
	EQUAL(sokoban.toString(), level);
	EQUAL(sokoban.getCellAt(Chthon::Point(0, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(1, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(5, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(6, 0)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(0, 4)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(1, 4)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(5, 4)).type, int(Cell::SPACE));
	EQUAL(sokoban.getCellAt(Chthon::Point(6, 4)).type, int(Cell::SPACE));
	int spaceCount = 0;
	for(int x = 0; x < sokoban.width(); ++x) {
		for(int y = 0; y < sokoban.height(); ++y) {
			if(sokoban.getCellAt(Chthon::Point(x, y)).type == Cell::SPACE) {
				spaceCount++;
			}
		}
	}
	EQUAL(spaceCount, 8);
}

}

int main(int argc, char ** argv)
{
	run_all_tests(argc, argv);
}

