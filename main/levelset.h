#pragma once
#include <QtCore/QString>

class LevelSet {
public:
	LevelSet(int startLevelIndex = 0);
	virtual ~LevelSet() {}
	bool moveToNextLevel();
	int getCurrentLevelIndex() const { return currentLevelIndex; }
	const QString & getCurrentLevel() const { return currentLevel; }
	bool isOver() const { return over; }
private:
	bool over;
	int currentLevelIndex;
	int levelCount;
	QString currentLevel;
	int currentSetPos;
};

