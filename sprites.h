#pragma once
#include <QtGui/QImage>
#include <QtCore/QMap>

class Sprites {
public:
	enum { FLOOR, WALL, EMPTY_SLOT, SPACE, PLAYER_ON_FLOOR, PLAYER_ON_SLOT, BOX_ON_FLOOR, BOX_ON_SLOT };
	Sprites(const QString & filename);
	QRect getSpriteRect(int tileType, int tileIndex) const;
	const QImage & getTileSet() const;
	QSize getSpritesBounds() const;
	bool contains(int tileType) const;
private:
	QImage tileset;
	QSize sprite_size;
	QMap<int, QList<QPoint> > cachedSprites;
};
