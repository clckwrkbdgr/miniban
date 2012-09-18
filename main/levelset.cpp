#include <QtDebug>
#include "levelset.h"
#include "levels.cpp"

LevelSet::LevelSet(int startLevelIndex)
	: over(false), currentLevelIndex(-1), levelCount(0), currentSetPos(0)
{
	levelCount = LEVELS.count("\n\n") + 1;
	startLevelIndex = qBound(0, startLevelIndex, levelCount - 1);
	currentLevelIndex = startLevelIndex - 1;
	while(startLevelIndex--) {
		currentSetPos = LEVELS.indexOf("\n\n", currentSetPos + 1);
	}

	moveToNextLevel();
}

bool LevelSet::moveToNextLevel()
{
	currentLevel.clear();
	if(currentSetPos < 0) {
		over = true;
		return false;
	}

	++currentLevelIndex;
	if(currentLevelIndex > 0) {
		currentSetPos += 2;
	}
	int endOfLevel = LEVELS.indexOf("\n\n", currentSetPos);
	int levelLength = (endOfLevel < 0) ? (LEVELS.length() - currentSetPos) : (endOfLevel - currentSetPos);
	currentLevel = LEVELS.mid(currentSetPos, levelLength);
	currentSetPos = endOfLevel;

	return true;
}

