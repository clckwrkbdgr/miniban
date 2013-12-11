#pragma once
#include "sokoban.h"
#include <QtCore/QString>

class LevelSet {
public:
	LevelSet(const QString & levelSetFileName = QString(), int startLevelIndex = 0);
	virtual ~LevelSet() {}

	bool moveToNextLevel();
	void rewindToLevel(int levelIndex);

	int getCurrentLevelIndex() const { return currentLevelIndex; }
	QString getCurrentLevelName() const;
	int getLevelCount() const;
	const QString & getLevelSetTitle() const;
	QString getCurrentLevelSet() const;
	const Sokoban & getCurrentSokoban() const { return currentSokoban; }
	bool isOver() const { return over; }
private:
	bool over;
	int currentLevelIndex;
	QString currentLevel;
	Sokoban currentSokoban;
	int currentSetPos;
	QString levelSetTitle;

	QString fileName;
	QList<QPair<QString, QString> > xmlLevels;

	bool loadFromFile(const QString & fileName);
};

