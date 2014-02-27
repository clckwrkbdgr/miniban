#include "levelset.h"
#include "xmlreader.h"
#include <chthon2/util.h>
#include <chthon2/log.h>
#include <sstream>
#include <fstream>
using Chthon::log;
using Chthon::format;

bool LevelSet::loadFromFile(const std::string & file_name, int startLevelIndex)
{
	if(file_name.empty()) {
		return false;
	}
	std::ifstream file(file_name.c_str(), std::ifstream::in);
	std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	this->file_name = file_name;
	return loadFromString(file_content, startLevelIndex);
}

bool LevelSet::loadFromString(const std::string & content, int startLevelIndex)
{
	std::istringstream in(content);
	XMLReader reader(in);
	reader.skip_to_tag("Title");
	reader.to_next_tag();
	levelSetTitle = reader.get_current_content();
	while(!reader.skip_to_tag("Level").empty()) {
		std::string level_name = reader.get_attributes()["Id"];
		std::string level_data;
		while(reader.to_next_tag() == "L") {
			reader.to_next_tag();
			level_data += reader.get_current_content() + '\n';
		}
		xml_levels.push_back(make_pair(level_name, level_data));
	}

	over = false;
	rewindToLevel(startLevelIndex);
	moveToNextLevel();
	return true;
}

LevelSet::LevelSet()
	: over(true), currentLevelIndex(-1)
{
}

void LevelSet::rewindToLevel(int level_index)
{
	currentLevelIndex = Chthon::bound(0, level_index, int(xml_levels.size())) - 1;
}

int LevelSet::getLevelCount() const
{
	return xml_levels.size();
}

const std::string & LevelSet::getLevelSetTitle() const
{
	return levelSetTitle;
}

std::string LevelSet::getCurrentLevelName() const
{
	if(currentLevelIndex < 0 || int(xml_levels.size()) <= currentLevelIndex) {
		return std::string();
	}
	return xml_levels[currentLevelIndex].first;
}

std::string LevelSet::getCurrentLevelSet() const
{
	return file_name;
}

bool LevelSet::moveToNextLevel()
{
	if(over) {
		return false;
	}
	++currentLevelIndex;
	if(currentLevelIndex >= int(xml_levels.size())) {
		over = true;
		return false;
	}
	currentLevel = xml_levels[currentLevelIndex].second;
	currentSokoban = Sokoban(currentLevel);
	return true;
}

