#pragma once
#include <QtCore/QMap>
#include <QtGui/QWidget>
#include "levelset.h"

class SokobanWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(SokobanWidget);
public:
	SokobanWidget(QWidget * parent = 0);
	virtual ~SokobanWidget() {}
signals:
	void wantsToQuit();
protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void resizeEvent(QResizeEvent*);
private:
	QMap<QChar, QImage> sprites;
	QSize spriteSize;
	QString currentLevel;
	QString history;
	LevelSet levelSet;
	QSize currentLevelSize;

	void restartLevel();
	void resizeSpritesForLevel(const QSize & levelSize);
	void showMessage(const QString & message);
	void loadNextLevel();
	void processControl(int control);
};

