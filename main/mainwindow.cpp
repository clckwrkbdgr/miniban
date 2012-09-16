#include "sokobanwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	QWidget * widget = new SokobanWidget();
	connect(widget, SIGNAL(wantsToQuit()), this, SLOT(close()));
	setCentralWidget(widget);
	widget->setFocus();
}
