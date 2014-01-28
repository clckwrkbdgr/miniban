#include <QtCore/QCoreApplication>
#include "sokobanwidget.h"

int main(int argc, char ** argv)
{
	QCoreApplication app(argc, argv);
	app.setOrganizationName("kp580bm1");
	app.setApplicationName("miniban");
	qsrand(time(NULL));

	SokobanWidget wnd;
	return wnd.exec();
}
