#include <QtGui/QPainter>
#include "sokobanwidget.h"

SokobanWidget::SokobanWidget(QWidget * parent)
	: QWidget(parent)
{
}

void SokobanWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);
}
