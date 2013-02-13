#pragma once
#include <QtCore/QString>

class LevelSet {
public:
	LevelSet(const QString & levelSetFileName = QString(), int startLevelIndex = 0);
	virtual ~LevelSet() {}

	bool moveToNextLevel();
	void rewindToLevel(int levelIndex);

	int getCurrentLevelIndex() const { return currentLevelIndex; }
	QString getCurrentLevelName() const;
	QString getCurrentLevelSet() const;
	const QString & getCurrentLevel() const { return currentLevel; }
	bool isOver() const { return over; }
private:
	bool usingEmbedded;

	bool over;
	int currentLevelIndex;
	QString currentLevel;
	int currentSetPos;

	QString fileName;
	QList<QPair<QString, QString> > xmlLevels;

	bool loadFromFile(const QString & fileName);
};

