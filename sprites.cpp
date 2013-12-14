#include <QtDebug>
#include <QtCore/QMap>
#include <QtGui/QImage>
#include "sokoban.h"
#include "sprites.h"

QImage get_tile(const QImage & tileset, int x, int y, const QSize & tile_size)
{
	return tileset.copy(x * tile_size.width(), y * tile_size.height(), tile_size.width(), tile_size.height());
}

Sprites::Sprites(const QString & filename)
	: tileset(filename)
{
	if(!tileset.isNull()) {
		return;
	}
	sprite_size = QSize(tileset.width() / 4, tileset.height() / 8);
	cachedSprites[Sprites::FLOOR]           << QPoint(0, 0) << QPoint(1, 0) << QPoint(2, 0) << QPoint(3, 0);
	cachedSprites[Sprites::WALL]            << QPoint(0, 1) << QPoint(1, 1) << QPoint(2, 1) << QPoint(3, 1);
	cachedSprites[Sprites::EMPTY_SLOT]      << QPoint(0, 2) << QPoint(1, 2) << QPoint(2, 2) << QPoint(3, 2);
	cachedSprites[Sprites::SPACE]           << QPoint(0, 3) << QPoint(1, 3) << QPoint(2, 3) << QPoint(3, 3);
	cachedSprites[Sprites::PLAYER_ON_FLOOR] << QPoint(0, 4) << QPoint(1, 4) << QPoint(2, 4) << QPoint(3, 4);
	cachedSprites[Sprites::PLAYER_ON_SLOT]  << QPoint(0, 5) << QPoint(1, 5) << QPoint(2, 5) << QPoint(3, 5);
	cachedSprites[Sprites::BOX_ON_FLOOR]    << QPoint(0, 6) << QPoint(1, 6) << QPoint(2, 6) << QPoint(3, 6);
	cachedSprites[Sprites::BOX_ON_SLOT]     << QPoint(0, 7) << QPoint(1, 7) << QPoint(2, 7) << QPoint(3, 7);
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
	if(!tileset.isNull()) {
		return false;
	}
	static QList<int> tileTypes = QList<int>()
		<< Sprites::FLOOR
		<< Sprites::WALL
		<< Sprites::EMPTY_SLOT
		<< Sprites::SPACE
		<< Sprites::PLAYER_ON_FLOOR
		<< Sprites::PLAYER_ON_SLOT
		<< Sprites::BOX_ON_FLOOR
		<< Sprites::BOX_ON_SLOT;
	return tileTypes.contains(tileType);
}
