#include "sokobanwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	setCentralWidget(new SokobanWidget());
}
