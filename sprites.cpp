#include <QtCore/QMap>
#include <QtGui/QImage>
#include "sokoban.h"
#include "sprites.h"
#include "xpm.h"

namespace Sprites { // Aux functions and cached sprites.

QMap<int, QImage> getAllConvertedSprites()
{
	QImage sprites(":/sprites");
	QMap<int, QImage> result;
	result[Sokoban::FLOOR]           = sprites.copy(0, 0, 8, 8);
	result[Sokoban::WALL]            = sprites.copy(8, 0, 8, 8);
	result[Sokoban::EMPTY_SLOT]      = sprites.copy(16, 0, 8, 8);
	result[Sokoban::SPACE]           = sprites.copy(24, 0, 8, 8);
	result[Sokoban::PLAYER_ON_FLOOR] = sprites.copy(0, 8, 8, 8);
	result[Sokoban::PLAYER_ON_SLOT]  = sprites.copy(8, 8, 8, 8);
	result[Sokoban::BOX_ON_FLOOR]    = sprites.copy(16, 8, 8, 8);
	result[Sokoban::BOX_ON_SLOT]     = sprites.copy(24, 8, 8, 8);
	return result;
}

static const QMap<int, QImage> cachedSprites = getAllConvertedSprites();

};

namespace Sprites { // Main.

QSize getSpritesBounds()
{
	return QSize(8, 8);
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
