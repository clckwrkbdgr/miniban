#include <QtCore/QCoreApplication>
#include "sokobanwidget.h"

int main(int argc, char ** argv)
{
	QCoreApplication app(argc, argv);
	app.setOrganizationName("kp580bm1");
	app.setApplicationName("miniban");
	qsrand(time(NULL));

	// TODO QCoreApplication::setOverrideCursor(Qt::BlankCursor);

	SokobanWidget wnd;
	//wnd.showFullScreen();
	return wnd.exec();
}
