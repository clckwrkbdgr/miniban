#pragma once
#include "sokoban.h"
#include <QtCore/QPair>
#include <QtCore/QString>

class LevelSet {
public:
	LevelSet();
	virtual ~LevelSet() {}

	bool loadFromFile(const QString & fileName, int startLevelIndex);
	bool loadFromString(const QString & content, int startLevelIndex);

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
	QString levelSetTitle;

	QString fileName;
	QList<QPair<QString, QString> > xmlLevels;
};

