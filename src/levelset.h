#pragma once
#include "sokoban.h"
#include <vector>
#include <string>

class LevelSet {
public:
	LevelSet();
	virtual ~LevelSet() {}

	bool loadFromFile(const std::string & file_name, int startLevelIndex);
	bool loadFromString(const std::string & content, int startLevelIndex);

	bool moveToNextLevel();
	void rewindToLevel(int levelIndex);

	int getCurrentLevelIndex() const { return currentLevelIndex; }
	std::string getCurrentLevelName() const;
	int getLevelCount() const;
	const std::string & getLevelSetTitle() const;
	std::string getCurrentLevelSet() const;
	const Sokoban & getCurrentSokoban() const { return currentSokoban; }
	bool isOver() const { return over; }
private:
	bool over;
	int currentLevelIndex;
	std::string currentLevel;
	Sokoban currentSokoban;
	std::string levelSetTitle;

	std::string file_name;
	std::vector<std::pair<std::string, std::string> > xml_levels;
};

