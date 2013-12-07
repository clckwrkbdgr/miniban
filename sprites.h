#pragma once
#include <QtGui/QImage>
#include <QtCore/QMap>

class Sprites {
public:
	enum { FLOOR, WALL, EMPTY_SLOT, SPACE, PLAYER_ON_FLOOR, PLAYER_ON_SLOT, BOX_ON_FLOOR, BOX_ON_SLOT };
	Sprites(const QString & filename);
	QImage getSprite(int tileType, int scaleFactor = 1) const;
	QSize getSpritesBounds() const;
private:
	QImage tileset;
	QSize sprite_size;
	QMap<int, QPoint> cachedSprites;
};
