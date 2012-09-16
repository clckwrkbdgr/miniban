#pragma once
#include <QtCore/QString>

class LevelSet {
public:
	LevelSet();
	virtual ~LevelSet() {}
	bool moveToNextLevel();
	const QString & getCurrentLevel() const { return currentLevel; }
	bool isOver() const { return over; }
private:
	bool over;
	QString currentLevel;
	int currentSetPos;
};

