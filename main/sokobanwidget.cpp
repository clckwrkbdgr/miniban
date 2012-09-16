#include <QtDebug>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include "sprites.h"
#include "sokoban.h"
#include "sokobanwidget.h"

const int SCALE_FACTOR = 4;

SokobanWidget::SokobanWidget(QWidget * parent)
	: QWidget(parent)
{
	QList<int> tileTypes;
	tileTypes << Sokoban::TileType::FLOOR;
	tileTypes << Sokoban::TileType::WALL;
	tileTypes << Sokoban::TileType::EMPTY_SLOT;
	tileTypes << Sokoban::TileType::PLAYER_ON_FLOOR;
	tileTypes << Sokoban::TileType::PLAYER_ON_SLOT;
	tileTypes << Sokoban::TileType::BOX_ON_FLOOR;
	tileTypes << Sokoban::TileType::BOX_ON_SLOT;

	foreach(int tileType, tileTypes) {
		sprites[tileType] = Sprites::getSprite(tileType, SCALE_FACTOR);
	}
}

void SokobanWidget::keyPressEvent(QKeyEvent * event)
{
	switch(event->key()) {
		case Qt::Key_Q: emit wantsToQuit(); break;
		default: QWidget::keyPressEvent(event);
	}
}

void SokobanWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);

	int xOffset = 0;
	foreach(const QImage & sprite, sprites) {
		painter.drawImage(xOffset, 0, sprite);
		xOffset += sprite.width();
	}
}
