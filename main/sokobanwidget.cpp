#include <QtDebug>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include "sprites.h"
#include "sokoban.h"
#include "sokobanwidget.h"

const int SCALE_FACTOR = 4;

SokobanWidget::SokobanWidget(QWidget * parent)
	: QWidget(parent)
{
	restartLevel();
}

const QList<int> & getAllTileTypes()
{
	static QList<int> tileTypes = QList<int>()
		<< Sokoban::TileType::FLOOR
		<< Sokoban::TileType::WALL
		<< Sokoban::TileType::EMPTY_SLOT
		<< Sokoban::TileType::PLAYER_ON_FLOOR
		<< Sokoban::TileType::PLAYER_ON_SLOT
		<< Sokoban::TileType::BOX_ON_FLOOR
		<< Sokoban::TileType::BOX_ON_SLOT;
	return tileTypes;
}

void SokobanWidget::resizeEvent(QResizeEvent*)
{
	foreach(int tileType, getAllTileTypes()) {
		sprites[tileType] = Sprites::getSprite(tileType, SCALE_FACTOR);
	}

	foreach(const QImage & sprite, sprites) {
		int width = sprite.width();
		int height = sprite.height();
		if(spriteSize.width() < width) {
			spriteSize.setWidth(width);
		}
		if(spriteSize.height() < height) {
			spriteSize.setHeight(height);
		}
	}
}

void SokobanWidget::restartLevel()
{
	currentLevel = levelSet.getCurrentLevel();
	history.clear();
}

void SokobanWidget::keyPressEvent(QKeyEvent * event)
{
	bool isShiftDown = event->modifiers().testFlag(Qt::ShiftModifier);
	bool isCtrlDown = event->modifiers().testFlag(Qt::ControlModifier);
	enum { NONE, QUIT, LEFT, RIGHT, UP, DOWN, UNDO, HOME };

	int key = NONE;
	switch(event->key()) {                                   //# CONTROLS
		case Qt::Key_Q: key = QUIT; break;                      //# 'q' or Ctrl-Q - quit.
		case Qt::Key_Left:  case Qt::Key_H: key = LEFT; break;  //# Left or 'h' - move left.
		case Qt::Key_Down:  case Qt::Key_J: key = DOWN; break;  //# Down or 'j' - move down.
		case Qt::Key_Up:    case Qt::Key_K: key = UP; break;    //# Up or 'k' - move .
		case Qt::Key_Right: case Qt::Key_L: key = RIGHT; break; //# Right or 'l' - move left.
		case Qt::Key_Z:    if(isCtrlDown) key = UNDO; break;    //# Ctrl-Z, Backspace or 'u' - undo last action.
		case Qt::Key_Backspace: key = UNDO; break;
		case Qt::Key_U:    key = isShiftDown ? HOME : UNDO; break;
		case Qt::Key_R:    if(isCtrlDown) key = HOME; break;    //# Ctrl-R, Home or 'U' - revert to the starting position.
		case Qt::Key_Home: key = HOME; break;
		default: QWidget::keyPressEvent(event); return;
	}

	switch(key) {
		case QUIT: emit wantsToQuit(); break;
		case LEFT:  currentLevel = Sokoban::process(currentLevel, Sokoban::Control::LEFT, &history); break;
		case DOWN:  currentLevel = Sokoban::process(currentLevel, Sokoban::Control::DOWN, &history); break;
		case UP:    currentLevel = Sokoban::process(currentLevel, Sokoban::Control::UP, &history); break;
		case RIGHT: currentLevel = Sokoban::process(currentLevel, Sokoban::Control::RIGHT, &history); break;
		case HOME: restartLevel(); break;
		case UNDO:
				   if(!history.isEmpty()) {
					   currentLevel = Sokoban::undo(currentLevel, &history);
				   }
				   break;
		default: break;
	}

	if(!levelSet.isOver()) {
		if(Sokoban::isSolved(currentLevel)) {
			bool ok = levelSet.moveToNextLevel();
			if(ok) {
				restartLevel();
			} else {
				showMessage(tr("Levels are over."));
			}
		}
	}

	update();
}

void SokobanWidget::showMessage(const QString & message)
{
	QMessageBox::information(this, tr("Sokoban"), message);
}

void SokobanWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);

	QStringList rows = currentLevel.split('\n');
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
