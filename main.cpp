#include "src/sokobanwidget.h"

int main(int argc, char ** argv)
{
	srand(time(NULL));

	SokobanWidget wnd(argc, argv);
	return wnd.exec();
}
