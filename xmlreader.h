#pragma once
#include <string>

struct XMLReader {
	XMLReader(std::istream & stream);

	const std::string & to_next_tag();

	const std::string & get_current_tag() const { return current_tag; }
	const std::string & get_current_content() const { return current_content; }
private:
	std::istream & s;
	std::string current_tag;
	std::string current_content;
};

