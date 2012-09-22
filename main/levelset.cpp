#include <QtDebug>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include "levelset.h"
#include "levels.cpp"

LevelSet::LevelSet(int startLevelIndex)
	: usingEmbedded(true), over(false), currentLevelIndex(-1), currentSetPos(0)
{
	rewindToLevel(startLevelIndex);
	moveToNextLevel();
}

bool LevelSet::loadFromFile(const QString & fileName)
{
	QDomDocument doc;
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return false;
	if(!doc.setContent(&file))
		return false;
	file.close();

	QDomElement root = doc.documentElement();
	QDomElement title = root.firstChildElement("Title");
	QDomElement levelCollection = root.firstChildElement("LevelCollection");
	QDomElement level = levelCollection.firstChildElement("Level");
	while(!level.isNull()) {
		QDomElement l = level.firstChildElement("L");
		QString levelData;
		while(!l.isNull()) {
			levelData += l.text();
			l = l.nextSiblingElement();
			if(!l.isNull()) {
				levelData += '\n';
			}
		}
		xmlLevels << qMakePair(title.text() + '/' + level.attribute("Id"), levelData);
		level = level.nextSiblingElement();
	}
	return true;
}

LevelSet::LevelSet(const QString & levelSetFileName, int startLevelIndex)
	: usingEmbedded(false), over(true), currentLevelIndex(-1), currentSetPos(0)
{
	if(!loadFromFile(levelSetFileName))
		return;
	over = false;
	rewindToLevel(startLevelIndex);
	moveToNextLevel();
}

void LevelSet::rewindToLevel(int levelIndex)
{
	if(usingEmbedded) {
		int levelCount = LEVELS.count("\n\n") + 1;
		levelIndex = qBound(0, levelIndex, levelCount - 1);
		currentLevelIndex = levelIndex - 1;
		while(levelIndex--) {
			currentSetPos = LEVELS.indexOf("\n\n", currentSetPos + 1);
		}
	} else {
		currentLevelIndex = qBound(0, levelIndex, xmlLevels.count() - 1) - 1;
	}
}

QString LevelSet::getCurrentLevelName() const
{
	if(usingEmbedded) {
		return QString::number(getCurrentLevelIndex());
	} else {
		if(currentLevelIndex >= xmlLevels.count()) {
			return QString();
		}
		return xmlLevels[currentLevelIndex].first;
	}
}

bool LevelSet::moveToNextLevel()
{
	if(usingEmbedded) {
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
	} else {
		++currentLevelIndex;
		if(currentLevelIndex >= xmlLevels.count()) {
			over = true;
			return false;
		}
		currentLevel = xmlLevels[currentLevelIndex].second;
	}
	return true;
}

