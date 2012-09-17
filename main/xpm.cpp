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

struct XPMInfo {
	int width, height, colors, charOnPixel;
	XPMInfo() : width(0), height(0), colors(0), charOnPixel(0) {}
	static XPMInfo fromString(const QString & string) {
		XPMInfo result;
		QList<int> values = extractValues(string);
		if(values.count() < 4)
			return result;
		result.width = values[0];
		result.height = values[1];
		result.colors = values[2];
		result.charOnPixel = values[3];
		return result;
	}
};

QMap<QString, QColor> getColorMap(const char ** colorTable, int colorCount, int charOnPixel)
{
	QMap<QString, QColor> colorMap;
	while(colorCount--) {
		QString colorLine = QString(*colorTable);
		QString name = colorLine.left(charOnPixel);
		colorLine.remove(0, charOnPixel);
		QStringList color = colorLine.split(' ', QString::SkipEmptyParts);
		if(color.count() < 2) {
			return QMap<QString, QColor>();
		}
		QString c = color[0];
		QString colorValue = color[1];

		colorMap[name] = colorValue;

		colorTable++;
	}
	return colorMap;
}

};

namespace XPM { // Main.

QImage toQImage(const char ** xpm)
{
	if(xpm == NULL)
		return QImage();

	XPMInfo values = XPMInfo::fromString(*xpm);
	xpm += 1;

	QMap<QString, QColor> colorMap = getColorMap(xpm, values.colors, values.charOnPixel);
	xpm += values.colors;
	if(colorMap.isEmpty()) {
		return QImage();
	}

	QImage image(values.width, values.height, QImage::Format_RGB32);
	QPoint pos;
	while(values.height--) {
		QString row = *xpm;
		while(!row.isEmpty()) {
			QString color = row.left(values.charOnPixel);
			image.setPixel(pos.x(), pos.y(), colorMap[color].rgb());
			pos.rx()++;
			row.remove(0, values.charOnPixel);
		}
		pos.ry()++;
		pos.rx() = 0;

		p++;
	}

	return image;
}

};
