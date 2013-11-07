#pragma once
#include <QtGui/QImage>
#include <QtCore/QMap>

class Sprites {
public:
	Sprites(const QString & filename);
	QImage getSprite(int tileType, int scaleFactor = 1) const;
	QSize getSpritesBounds() const;
private:
	QImage tileset;
	QSize sprite_size;
	QMap<int, QPoint> cachedSprites;
};
