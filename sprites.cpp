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
	sprite_size = QSize(tileset.width() / 4, tileset.height() / 2);
	cachedSprites[Sokoban::FLOOR]           = QPoint(0, 0);
	cachedSprites[Sokoban::WALL]            = QPoint(1, 0);
	cachedSprites[Sokoban::EMPTY_SLOT]      = QPoint(2, 0);
	cachedSprites[Sokoban::SPACE]           = QPoint(3, 0);
	cachedSprites[Sokoban::PLAYER_ON_FLOOR] = QPoint(0, 1);
	cachedSprites[Sokoban::PLAYER_ON_SLOT]  = QPoint(1, 1);
	cachedSprites[Sokoban::BOX_ON_FLOOR]    = QPoint(2, 1);
	cachedSprites[Sokoban::BOX_ON_SLOT]     = QPoint(3, 1);
}

QSize Sprites::getSpritesBounds() const
{
	return sprite_size;
}

QImage Sprites::getSprite(int tileType, int scaleFactor) const
{
	scaleFactor = (scaleFactor < 1) ? 1 : scaleFactor;
	if(cachedSprites.contains(tileType)) {
		QPoint tile_pos = cachedSprites[tileType];
		QRect r(QPoint(tile_pos.x() * sprite_size.width(), tile_pos.y() * sprite_size.height()), sprite_size);
		return tileset.copy(r).scaled(sprite_size * scaleFactor);
	}
	return QImage();
}

