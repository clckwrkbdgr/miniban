#include "../src/levelset.h"
#include <chthon/test.h>
using namespace Chthon::UnitTest;

static const char * xml =  
"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
"<SokobanLevels xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"SokobanLev.xsd\">\n"
"  <Title>Miniban</Title>\n"
"  <Description>\n"
" Some description.\n"
"  </Description>\n"
"  <Email>miniban@example.com</Email>\n"
"  <Url>http://example.com/</Url>\n"
"  <LevelCollection Copyright=\"Miniban\" MaxWidth=\"47\" MaxHeight=\"41\">\n"
"    <Level Id=\"One\" Width=\"6\" Height=\"8\">\n"
"      <L>####</L>\n"
"      <L>#  ###</L>\n"
"      <L>#.$ @#</L>\n"
"      <L>#  ###</L>\n"
"      <L>####</L>\n"
"    </Level>\n"
"    <Level Id=\"Two\" Width=\"6\" Height=\"17\">\n"
"      <L> #####</L>\n"
"      <L>#  @ #</L>\n"
"      <L>#  $ #</L>\n"
"      <L># #.##</L>\n"
"      <L>#####</L>\n"
"    </Level>\n"
"    <Level Id=\"Three\" Width=\"7\" Height=\"8\">\n"
"      <L>   ####</L>\n"
"      <L>####  #</L>\n"
"      <L># @$. #</L>\n"
"      <L>#######</L>\n"
"    </Level>\n"
"  </LevelCollection>\n"
" </SokobanLevels>\n"
;

SUITE(levelset) {

TEST(should_load_levelset_title)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 0);
	EQUAL(levelset.getLevelSetTitle(), "Miniban");
}

TEST(should_load_all_levels)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 0);
	EQUAL(levelset.getLevelCount(), 3);
}

TEST(should_load_levels_as_they_are)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 0);
	const char * level =
		"####  \n"
		"#  ###\n"
		"#.$ @#\n"
		"#  ###\n"
		"####  "
		;
	EQUAL(levelset.getCurrentSokoban().toString(), level);
}

TEST(should_load_level_names)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 0);
	EQUAL(levelset.getCurrentLevelName(), "One");
}

TEST(should_point_at_specified_level_after_loading)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 1);
	EQUAL(levelset.getCurrentLevelIndex(), 1);
}

TEST(should_move_to_the_next_level)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 1);
	levelset.moveToNextLevel();
	EQUAL(levelset.getCurrentLevelIndex(), 2);
}

TEST(should_be_over_when_there_is_no_next_level)
{
	LevelSet levelset;
	levelset.loadFromString(xml, 1);
	bool ok = levelset.moveToNextLevel();
	ASSERT(ok);
	levelset.moveToNextLevel();
	ASSERT(levelset.isOver());
}

}
