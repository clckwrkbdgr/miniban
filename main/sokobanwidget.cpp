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

	foreach(const QImage & sprite, sprites) {
		if(spriteSize.width() < sprite.size().width()) {
			spriteSize.setWidth(sprite.size().width());
		}
		if(spriteSize.height() < sprite.size().height()) {
			spriteSize.setHeight(sprite.size().height());
		}
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

	QString level = 
		"####\n"
		"#  ############\n"
		"# $ $ $ $ $ @ #\n"
		"# .....       #\n"
		"###############\n"
		;

	QStringList rows = level.split('\n');
	int levelWidth = 0;
	int levelHeight = rows.count();
	foreach(const QString & row, rows) {
		if(levelWidth < row.length()) {
			levelWidth = row.length();
		}
	}

	QPoint offset = QPoint(width() - levelWidth * spriteSize.width(), height() - levelHeight * spriteSize.height()) / 2;
	for(int y = 0; y < levelHeight; ++y) {
		for(int x = 0; x < levelWidth && x < rows[y].length(); ++x) {
			QChar tileType = sprites.contains(rows[y][x]) ? rows[y][x] : QChar(Sokoban::TileType::FLOOR);
			QPoint pos = offset + QPoint(x * spriteSize.width(), y * spriteSize.height());
			painter.drawImage(pos, sprites[tileType]);
		}
	}
}
