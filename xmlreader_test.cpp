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

TEST(should_skip_whitespace_before_tag_name)
{
	std::istringstream stream("< Hello>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_tag(), "Hello");
}

TEST(should_skip_whitespace_after_tag_name)
{
	std::istringstream stream("<Hello >");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_tag(), "Hello");
}

TEST(should_read_tag_name_as_one_identifier)
{
	std::istringstream stream("<Hello attribute=value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_tag(), "Hello");
}

TEST(should_read_attribute)
{
	std::istringstream stream("<Hello attribute=value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_skip_whitespace_after_attribute_name)
{
	std::istringstream stream("<Hello attribute =value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_skip_whitespace_before_attribute_value)
{
	std::istringstream stream("<Hello attribute= value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_skip_whitespace_after_attribute_value)
{
	std::istringstream stream("<Hello attribute=value  >");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_read_more_than_one_attribute)
{
	std::istringstream stream("<Hello attribute=value second=two>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
	EQUAL(reader.get_attributes()["second"], "two");
}

TEST(should_read_quoted_attribute)
{
	std::istringstream stream("<Hello attribute=\"hello \\\"world\\\"\">");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "hello \"world\"");
}

}

