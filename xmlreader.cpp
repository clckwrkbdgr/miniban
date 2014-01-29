#include "xmlreader.h"
#include <sstream>

XMLReader::XMLReader(std::istream & stream)
	: s(stream)
{
	s.unsetf(std::ios::skipws);
}

const std::string & XMLReader::to_next_tag()
{
	current_tag.clear();
	current_content.clear();
	attributes.clear();

	char ch;
	s >> ch;
	while(ch != '<') {
		current_content += ch;
		s >> ch;
	}

	s >> ch;
	while(ch != '>') {
		current_tag += ch;
		s >> ch;
	}
	return current_tag;
}

