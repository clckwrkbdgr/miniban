#include <QtGui/QApplication>
#include "sokobanwidget.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("kp580bm1");
	app.setApplicationName("miniban");
	qsrand(time(NULL));

	QApplication::setOverrideCursor(Qt::BlankCursor);

	SokobanWidget wnd;
	wnd.showFullScreen();
	return app.exec();
}
