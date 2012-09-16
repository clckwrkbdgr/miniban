#include <QtDebug>
#include "levelset.h"
#include "levels.cpp"

LevelSet::LevelSet()
	: over(false), currentSetPos(0)
{
	moveToNextLevel();
}

QString trimLN(const QString & string)
{
	QString result = string;
	while(result.startsWith('\n')) {
		result.remove(0, 1);
	}
	while(result.endsWith('\n')) {
		result.chop(1);
	}
	return result;
}

bool LevelSet::moveToNextLevel()
{
	currentLevel.clear();
	if(currentSetPos < 0) {
		over = true;
		return false;
	}

	while(currentSetPos > -1) {
		int newPos = LEVELS.indexOf('\n', currentSetPos + 1);
		bool endOfLevel = (newPos <= currentSetPos + 1);
		currentLevel += trimLN(LEVELS.mid(currentSetPos, newPos - currentSetPos)) + '\n';
		currentSetPos = newPos;
		if(endOfLevel) {
			break;
		}
	}
	currentLevel = trimLN(currentLevel);
	return true;
}

