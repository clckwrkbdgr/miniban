#pragma once
#include <QtGui/QWidget>

class SokobanWidget : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(SokobanWidget);
public:
	SokobanWidget(QWidget * parent = 0);
	virtual ~SokobanWidget() {}
protected:
	virtual void paintEvent(QPaintEvent*);
};

