#include <QtDebug>
#include "sprites.h"
#include "sokoban.h"
#include <QtGui/QImage>
#include <QtCore/QMap>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <chthon/pixmap.h>

QImage get_tile(const QImage & tileset, int x, int y, const QSize & tile_size)
{
	return tileset.copy(x * tile_size.width(), y * tile_size.height(), tile_size.width(), tile_size.height());
}

Sprites::Sprites(const QString & filename)
{
	QTextStream err(stderr);
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		err << QObject::tr("Cannot open file <%1>.").arg(filename) << endl;
		return;
	}

	QTextStream in(&file);
	QString file_content = in.readAll();

	try {
		Chthon::Pixmap pixmap(file_content.toStdString());
		tileset = QImage(pixmap.width(), pixmap.height(), QImage::Format_ARGB32);
		for(unsigned x = 0; x < pixmap.width(); ++x) {
			for(unsigned y = 0; y < pixmap.height(); ++y) {
				tileset.setPixel(x, y, pixmap.color(pixmap.pixel(x, y)).argb());
			}
		}
	} catch(const Chthon::Pixmap::Exception & e) {
		err << QString::fromStdString(e.what) << endl;
	}

	if(tileset.isNull()) {
		return;
	}
	cachedSprites[Sprites::FLOOR]           << QPoint(0, 0) << QPoint(1, 0) << QPoint(2, 0) << QPoint(3, 0);
	cachedSprites[Sprites::WALL]            << QPoint(0, 1) << QPoint(1, 1) << QPoint(2, 1) << QPoint(3, 1);
	cachedSprites[Sprites::EMPTY_SLOT]      << QPoint(0, 2) << QPoint(1, 2) << QPoint(2, 2) << QPoint(3, 2);
	cachedSprites[Sprites::SPACE]           << QPoint(0, 3) << QPoint(1, 3) << QPoint(2, 3) << QPoint(3, 3);
	cachedSprites[Sprites::PLAYER_ON_FLOOR] << QPoint(0, 4) << QPoint(1, 4) << QPoint(2, 4) << QPoint(3, 4);
	cachedSprites[Sprites::PLAYER_ON_SLOT]  << QPoint(0, 5) << QPoint(1, 5) << QPoint(2, 5) << QPoint(3, 5);
	cachedSprites[Sprites::BOX_ON_FLOOR]    << QPoint(0, 6) << QPoint(1, 6) << QPoint(2, 6) << QPoint(3, 6);
	cachedSprites[Sprites::BOX_ON_SLOT]     << QPoint(0, 7) << QPoint(1, 7) << QPoint(2, 7) << QPoint(3, 7);
	cachedSprites[Sprites::CURSOR]          << QPoint(0, 8);
	int max_x = 0, max_y = 0;
	foreach(int key, cachedSprites.keys()) {
		foreach(const QPoint & point, cachedSprites[key]) {
			max_x = qMax(point.x(), max_x);
			max_y = qMax(point.y(), max_y);
		}
	}
	sprite_size = QSize(tileset.width() / (max_x + 1), tileset.height() / (max_y + 1));
}

QSize Sprites::getSpritesBounds() const
{
	if(tileset.isNull()) {
		return QSize();
	}
	return sprite_size;
}

QRect Sprites::getSpriteRect(int tileType, int spriteIndex) const
{
	if(cachedSprites.contains(tileType)) {
		QPoint tile_pos = cachedSprites[tileType].value(spriteIndex);
		return QRect(QPoint(tile_pos.x() * sprite_size.width(), tile_pos.y() * sprite_size.height()), sprite_size);
	}
	return QRect();
}

const QImage & Sprites::getTileSet() const
{
	return tileset;
}

bool Sprites::contains(int tileType) const
{
	if(tileset.isNull()) {
		return false;
	}
	return cachedSprites.contains(tileType);
}
