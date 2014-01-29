#include "xmlreader.h"
#include <chthon/files.h>
#include <chthon/log.h>
#include <chthon/format.h>
#include <sstream>
#include <cctype>
using Chthon::format;
using Chthon::log;

XMLReader::XMLReader(std::istream & stream)
	: s(stream)
{
	s.unsetf(std::ios::skipws);
}

const std::string & XMLReader::skip_to_tag(const std::string & tag_name)
{
	to_next_tag();
	while(!current_tag.empty() && current_tag != tag_name) {
		to_next_tag();
	}
	return current_tag;
}

const std::string & XMLReader::to_next_tag()
{
	current_tag.clear();
	current_content.clear();
	attributes.clear();

	char ch;
	s >> ch;
	while(s && ch != '<') {
		current_content += ch;
		s >> ch;
	}

	enum { ERROR, TAG_NAME, ATTRIBUTE, EQUALS, VALUE };
	std::string attribute;
	int mode = TAG_NAME;
	s >> ch;
	while(s && ch != '>') {
		switch(mode) {
			case TAG_NAME:
				if(isspace(ch)) {
					if(!current_tag.empty()) {
						mode = ATTRIBUTE;
						attribute.clear();
					}
				} else {
					current_tag += ch;
				}
				break;
			case ATTRIBUTE:
				if(isspace(ch)) {
					if(!attribute.empty()) {
						mode = EQUALS;
					}
				} else {
					if(ch == '=') {
						mode = VALUE;
					} else {
						attribute += ch;
					}
				}
				break;
			case EQUALS:
				if(!isspace(ch)) {
					if(ch == '=') {
						mode = VALUE;
					} else {
						mode = ERROR;
					}
				}
				break;
			case VALUE:
				if(isspace(ch)) {
					if(attributes.count(attribute) > 0) {
						mode = ATTRIBUTE;
						attribute.clear();
					}
				} else {
					if(attributes.count(attribute) == 0 && ch == '"') {
						s.seekg(-1, s.cur); // TODO replace with aware of that read_string routine.
						attributes[attribute] = Chthon::read_string(s);
						mode = ATTRIBUTE;
						attribute.clear();
					} else {
						attributes[attribute] += ch;
					}
				}
				break;
			case ERROR: default: break;
		}
		s >> ch;
	}
	return current_tag;
}

