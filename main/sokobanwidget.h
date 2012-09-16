#pragma once
#include <QtCore/QMap>
#include <QtGui/QWidget>

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
private:
	QMap<QChar, QImage> sprites;
	QSize spriteSize;
	QString originalLevel, currentLevel;
	QString history;
};

