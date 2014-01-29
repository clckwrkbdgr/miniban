#include <QtDebug>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include "levelset.h"

bool LevelSet::loadFromFile(const QString & fileName, int startLevelIndex)
{
	if(fileName.isEmpty()) {
		return false;
	}
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return false;
	QTextStream in(&file);
	this->fileName = fileName;
	bool ok = loadFromString(in.readAll(), startLevelIndex);
	file.close();
	return ok;
}

bool LevelSet::loadFromString(const QString & content, int startLevelIndex)
{
	QDomDocument doc;
	if(!doc.setContent(content)) {
		return false;
	}
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
		xmlLevels << qMakePair(level.attribute("Id"), levelData);
		level = level.nextSiblingElement();
	}
	levelSetTitle = title.text();

	over = false;
	rewindToLevel(startLevelIndex);
	moveToNextLevel();
	return true;
}

LevelSet::LevelSet()
	: over(true), currentLevelIndex(-1)
{
}

void LevelSet::rewindToLevel(int levelIndex)
{
	currentLevelIndex = qBound(0, levelIndex, xmlLevels.count()) - 1;
}

int LevelSet::getLevelCount() const
{
	return xmlLevels.count();
}

const QString & LevelSet::getLevelSetTitle() const
{
	return levelSetTitle;
}

QString LevelSet::getCurrentLevelName() const
{
	if(currentLevelIndex < 0 || xmlLevels.count() <= currentLevelIndex) {
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
	if(over) {
		return false;
	}
	++currentLevelIndex;
	if(currentLevelIndex >= xmlLevels.count()) {
		over = true;
		return false;
	}
	currentLevel = xmlLevels[currentLevelIndex].second;
	currentSokoban = Sokoban(currentLevel.toStdString());
	return true;
}

