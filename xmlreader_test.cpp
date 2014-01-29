#include "xmlreader.h"
#include <chthon/test.h>
#include <sstream>
using namespace Chthon::UnitTest;

SUITE(xml) {

TEST(should_find_first_tag)
{
	std::istringstream stream("Hello<world>");
	XMLReader reader(stream);
	std::string first_tag = reader.to_next_tag();
	EQUAL(first_tag, "world");
}

TEST(should_find_next_tag)
{
	std::istringstream stream("<Hello><world>");
	XMLReader reader(stream);
	std::string first_tag = reader.to_next_tag();
	EQUAL(first_tag, "Hello");
	std::string next_tag = reader.to_next_tag();
	EQUAL(next_tag, "world");
}

TEST(should_store_intertag_content)
{
	std::istringstream stream("<Hello>content<world>");
	XMLReader reader(stream);
	reader.to_next_tag();
	reader.to_next_tag();
	EQUAL(reader.get_current_content(), "content");
}

}

