#include <QtDebug>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include "sprites.h"
#include "sokoban.h"
#include "sokobanwidget.h"

namespace { // Aux functions.

const int MIN_SCALE_FACTOR = 1;
const int MAX_SCALE_FACTOR = 8;
const int MAX_CHAR_COUNT_IN_MESSAGE = 60;

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

QSize calculateLevelSize(const QString & level)
{
	int levelWidth = 0;
	int levelHeight = 1;
	int lineWidth = 0;
	foreach(const QChar & c, level) {
		if(c == '\n') {
			++levelHeight;
			if(levelWidth < lineWidth) {
				levelWidth = lineWidth;
			}
			lineWidth = 0;
		} else {
			++lineWidth;
		}
	}
	return QSize(levelWidth, levelHeight);
}

enum { MODE_GAME, MODE_MESSAGE };
enum { CONTROL_NONE, CONTROL_QUIT, CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN, CONTROL_UNDO, CONTROL_HOME };


}

SokobanWidget::SokobanWidget(QWidget * parent)
	: QWidget(parent), mode(MODE_GAME)
{
	restartLevel();
}

void SokobanWidget::resizeEvent(QResizeEvent*)
{
	resizeSpritesForLevel(currentLevelSize);
}

void SokobanWidget::resizeSpritesForLevel(const QSize & levelSize)
{
	QSize originalSize = Sprites::getSpritesBounds();
	int scaleFactor = qMin(
			rect().width() / (levelSize.width() * originalSize.width()),
			rect().height() / (levelSize.height() * originalSize.height())
			);
	scaleFactor = qBound(MIN_SCALE_FACTOR, scaleFactor, MAX_SCALE_FACTOR);

	foreach(int tileType, getAllTileTypes()) {
		sprites[tileType] = Sprites::getSprite(tileType, scaleFactor);
	}

	spriteSize = originalSize * scaleFactor;
}

void SokobanWidget::restartLevel()
{
	currentLevel = levelSet.getCurrentLevel();
	history.clear();
	currentLevelSize = calculateLevelSize(currentLevel);
	resizeSpritesForLevel(currentLevelSize);
}

void SokobanWidget::processControl(int control)
{
	if(mode == MODE_MESSAGE) {
		if(control == CONTROL_QUIT) {
			wantsToQuit();
		}
		return;
	}

	switch(control) {
		case CONTROL_QUIT: emit wantsToQuit(); return;
		case CONTROL_LEFT:  currentLevel = Sokoban::process(currentLevel, Sokoban::Control::LEFT, &history); break;
		case CONTROL_DOWN:  currentLevel = Sokoban::process(currentLevel, Sokoban::Control::DOWN, &history); break;
		case CONTROL_UP:    currentLevel = Sokoban::process(currentLevel, Sokoban::Control::UP, &history); break;
		case CONTROL_RIGHT: currentLevel = Sokoban::process(currentLevel, Sokoban::Control::RIGHT, &history); break;
		case CONTROL_HOME: restartLevel(); break;
		case CONTROL_UNDO:
				   if(!history.isEmpty()) {
					   currentLevel = Sokoban::undo(currentLevel, &history);
				   }
				   break;
		default: return;
	}

	if(!levelSet.isOver()) {
		if(Sokoban::isSolved(currentLevel)) {
			loadNextLevel();
		}
	}
}

void SokobanWidget::keyPressEvent(QKeyEvent * event)
{
	//TODO temp only: a debug command to flip levels.
	if(mode == MODE_GAME && event->key() == Qt::Key_Space) {
		loadNextLevel();
		update();
		return;
	}

	bool isShiftDown = event->modifiers().testFlag(Qt::ShiftModifier);
	bool isCtrlDown = event->modifiers().testFlag(Qt::ControlModifier);

	//# CONTROLS
	int control = CONTROL_NONE;
	switch(event->key()) { //#
		case Qt::Key_Q: control = CONTROL_QUIT; break;                      //# 'q' or Ctrl-Q - quit.
		case Qt::Key_Left:  case Qt::Key_H: control = CONTROL_LEFT; break;  //# Left or 'h' - move left.
		case Qt::Key_Down:  case Qt::Key_J: control = CONTROL_DOWN; break;  //# Down or 'j' - move down.
		case Qt::Key_Up:    case Qt::Key_K: control = CONTROL_UP; break;    //# Up or 'k' - move .
		case Qt::Key_Right: case Qt::Key_L: control = CONTROL_RIGHT; break; //# Right or 'l' - move left.
		case Qt::Key_Z:    if(isCtrlDown) control = CONTROL_UNDO; break;    //# Ctrl-Z, Backspace or 'u' - undo last action.
		case Qt::Key_Backspace: control = CONTROL_UNDO; break;
		case Qt::Key_U:    control = isShiftDown ? CONTROL_HOME : CONTROL_UNDO; break;
		case Qt::Key_R:    if(isCtrlDown) control = CONTROL_HOME; break;    //# Ctrl-R, Home or 'U' - revert to the starting position.
		case Qt::Key_Home: control = CONTROL_HOME; break;
		default: QWidget::keyPressEvent(event); return;
	} //#

	processControl(control);
	update();
}

void SokobanWidget::loadNextLevel()
{
	bool ok = levelSet.moveToNextLevel();
	if(ok) {
		restartLevel();
	} else {
		showMessage(tr("Levels are over."));
	}
}

void SokobanWidget::showMessage(const QString & message)
{
	mode = MODE_MESSAGE;
	messageToShow = message;
}

void SokobanWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);

	if(mode == MODE_MESSAGE) {
		painter.setPen(Qt::white);
		QFont f = painter.font();
		f.setPixelSize(rect().width() / MAX_CHAR_COUNT_IN_MESSAGE);
		qDebug() << rect().width() << MAX_CHAR_COUNT_IN_MESSAGE << (rect().width() / MAX_CHAR_COUNT_IN_MESSAGE);
		painter.setFont(f);
		painter.drawText(rect(), Qt::AlignCenter, messageToShow);
		return;
	}

	QStringList rows = currentLevel.split('\n');

	QPoint offset = QPoint(
			width() - currentLevelSize.width() * spriteSize.width(),
			height() - currentLevelSize.height() * spriteSize.height()
			) / 2;
	for(int y = 0; y < currentLevelSize.height(); ++y) {
		for(int x = 0; x < currentLevelSize.width(); ++x) {
			QPoint pos = offset + QPoint(x * spriteSize.width(), y * spriteSize.height());
			bool validSprite = sprites.contains(rows[y][x]);
			bool stillInRow = x < rows[y].length();
			QChar tileType = (validSprite && stillInRow) ? rows[y][x] : QChar(Sokoban::TileType::FLOOR);
			painter.drawImage(pos, sprites[tileType]);
		}
	}
}
