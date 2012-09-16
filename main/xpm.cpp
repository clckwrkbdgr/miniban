#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtGui/QImage>
#include <QtGui/QColor>
#include "xpm.h"

namespace XPM { // Aux functions.

QList<int> extractValues(const QString & valuesString)
{
	QList<int> result;
	QStringList values = valuesString.split(' ');
	bool ok = false;
	foreach(const QString & string, values) {
		int value = string.toInt(&ok);
		if(!ok) {
			break;
		}
		result << value;
	}
	return result;
}

};

namespace XPM { // Main.

QImage toQImage(const char ** xpm)
{
	if(xpm == NULL)
		return QImage();
	const char ** p = xpm;

	QList<int> values = extractValues(*p);
	if(values.count() < 4)
		return QImage();
	int width = values[0];
	int height = values[1];
	int colors = values[2];
	int charOnPixel = values[3];

	QMap<QString, QColor> colorMap;
	while(colors--) {
		p++;
		QString colorLine = QString(*p);
		QString name = colorLine.left(charOnPixel);
		colorLine.remove(0, charOnPixel);
		QStringList color = colorLine.split(' ', QString::SkipEmptyParts);
		if(color.count() < 2) {
			return QImage();
		}
		QString c = color[0];
		QString colorValue = color[1];

		colorMap[name] = colorValue;
	}

	QImage image(width, height, QImage::Format_RGB32);
	QPoint pos;
	while(height--) {
		p++;
		QString row = *p;
		while(!row.isEmpty()) {
			QString color = row.left(charOnPixel);
			image.setPixel(pos.x(), pos.y(), colorMap[color].rgb());
			pos.rx()++;
			row.remove(0, charOnPixel);
		}
		pos.ry()++;
		pos.rx() = 0;
	}

	return image;
}

};
