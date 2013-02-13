#include <QtDebug>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include "levelset.h"
#include "levels.cpp"

bool LevelSet::loadFromFile(const QString & fileName)
{
	if(fileName.isEmpty()) {
		return false;
	}
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
	this->fileName = fileName;
	return true;
}

LevelSet::LevelSet(const QString & levelSetFileName, int startLevelIndex)
	: over(true), currentLevelIndex(-1), currentSetPos(0)
{
	if(!loadFromFile(levelSetFileName))
		return;
	over = false;
	rewindToLevel(startLevelIndex);
	moveToNextLevel();
}

void LevelSet::rewindToLevel(int levelIndex)
{
	currentLevelIndex = qBound(0, levelIndex, xmlLevels.count() - 1) - 1;
}

QString LevelSet::getCurrentLevelName() const
{
	if(currentLevelIndex >= xmlLevels.count()) {
		return QString();
	}
	return xmlLevels[currentLevelIndex].first;
}

QString LevelSet::getCurrentLevelSet() const
{
	return fileName;
}

bool LevelSet::moveToNextLevel()
{
	++currentLevelIndex;
	if(currentLevelIndex >= xmlLevels.count()) {
		over = true;
		return false;
	}
	currentLevel = xmlLevels[currentLevelIndex].second;
	return true;
}

