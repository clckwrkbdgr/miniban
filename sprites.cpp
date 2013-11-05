#include <QtDebug>
#include <QtCore/QMap>
#include <QtGui/QImage>
#include "sokoban.h"
#include "sprites.h"
#include "xpm.h"

namespace Sprites { // Aux functions and cached sprites.

QImage get_tile(const QImage & tileset, int x, int y, const QSize & tile_size)
{
	return tileset.copy(x * tile_size.width(), y * tile_size.height(), tile_size.width(), tile_size.height());
}

QMap<int, QImage> getAllConvertedSprites()
{
	QImage sprites("sokoban.png");
	QSize sprite_size = QSize(sprites.width() / 4, sprites.height() / 2);
	QMap<int, QImage> result;
	result[Sokoban::FLOOR]           = get_tile(sprites, 0, 0, sprite_size);
	result[Sokoban::WALL]            = get_tile(sprites, 1, 0, sprite_size);
	result[Sokoban::EMPTY_SLOT]      = get_tile(sprites, 2, 0, sprite_size);
	result[Sokoban::SPACE]           = get_tile(sprites, 3, 0, sprite_size);
	result[Sokoban::PLAYER_ON_FLOOR] = get_tile(sprites, 0, 1, sprite_size);
	result[Sokoban::PLAYER_ON_SLOT]  = get_tile(sprites, 1, 1, sprite_size);
	result[Sokoban::BOX_ON_FLOOR]    = get_tile(sprites, 2, 1, sprite_size);
	result[Sokoban::BOX_ON_SLOT]     = get_tile(sprites, 3, 1, sprite_size);
	return result;
}

static const QMap<int, QImage> cachedSprites = getAllConvertedSprites();

};

namespace Sprites { // Main.

QSize getSpritesBounds()
{
	return QSize(16, 16);
}

QImage getSprite(int tileType, int scaleFactor)
{
	scaleFactor = (scaleFactor < 1) ? 1 : scaleFactor;
	if(cachedSprites.contains(tileType)) {
		QImage result = cachedSprites[tileType];
		return result.scaled(result.size() * scaleFactor);
	}
	return QImage();
}

};
