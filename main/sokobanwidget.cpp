#include <QtDebug>
#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include "playingmode.h"
#include "messagemode.h"
#include "fademode.h"
#include "sokobanwidget.h"

SokobanWidget::SokobanWidget(QWidget * parent)
	: QWidget(parent), gameMode(0)
{
	QSettings settings;
	int lastLevelIndex = settings.value("levels/lastindex", 0).toInt();
	if(lastLevelIndex) {
		levelSet = LevelSet(lastLevelIndex);
	}

	startFadeIn();
}

SokobanWidget::~SokobanWidget()
{
	QSettings settings;
	settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());
}

void SokobanWidget::resizeEvent(QResizeEvent*)
{
	gameMode->invalidateRect();
}

void SokobanWidget::keyPressEvent(QKeyEvent * event)
{
	bool isShiftDown = event->modifiers().testFlag(Qt::ShiftModifier);
	bool isCtrlDown = event->modifiers().testFlag(Qt::ControlModifier);
	PlayingMode * playingMode = dynamic_cast<PlayingMode*>(gameMode);

	//# CONTROLS
	int control = AbstractGameMode::CONTROL_NONE;
	switch(event->key()) { //#
		case Qt::Key_0: // CHEAT CODE: restart leveset.
			if(playingMode) {
				levelSet = LevelSet(0);
				startFadeIn();
				return;
			}
			break;
		case Qt::Key_1: // CHEAT CODE: go to next level.
			if(playingMode) {
				loadNextLevel();
				return;
			}
			break;
		case Qt::Key_Q: //# 'q' or Ctrl-Q - quit.
			emit wantsToQuit();
			return;
		case Qt::Key_O: //# 'o' or Ctrl-O - open another level set.
			openLevelSet();
			return;
		case Qt::Key_Space: //# Space - skip intelevel message.
			control = AbstractGameMode::CONTROL_SKIP;
			break;
		case Qt::Key_Left:
		case Qt::Key_H: //# Left or 'h' - move left.
			control = AbstractGameMode::CONTROL_LEFT;
			break;
		case Qt::Key_Down:
		case Qt::Key_J: //# Down or 'j' - move down.
			control = AbstractGameMode::CONTROL_DOWN;
			break;
		case Qt::Key_Up:
		case Qt::Key_K: //# Up or 'k' - move up.
			control = AbstractGameMode::CONTROL_UP;
			break;
		case Qt::Key_Right:
		case Qt::Key_L: //# Right or 'l' - move right.
			control = AbstractGameMode::CONTROL_RIGHT;
			break;
		case Qt::Key_Z: //# Ctrl-Z, Backspace or 'u' - undo last action.
			if(isCtrlDown)
				control = AbstractGameMode::CONTROL_UNDO;
			break;
		case Qt::Key_Backspace:
			control = AbstractGameMode::CONTROL_UNDO;
			break;
		case Qt::Key_U:
			control = isShiftDown ?  AbstractGameMode::CONTROL_HOME : AbstractGameMode::CONTROL_UNDO;
			break;
		case Qt::Key_R: //# Ctrl-R, Home or 'U' - revert to the starting position.
			if(isCtrlDown)
				control = AbstractGameMode::CONTROL_HOME;
			break;
		case Qt::Key_Home:
			control = AbstractGameMode::CONTROL_HOME;
			break;
		default:
			QWidget::keyPressEvent(event);
			return;
	} //#

	gameMode->processControl(control);
	update();
}

void SokobanWidget::openLevelSet()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open sokoban level set"), "", tr("Sokoban level collections (*.slc)"));
	if(fileName.isEmpty())
		return;
	levelSet = LevelSet(fileName);
	startFadeIn();
}

void SokobanWidget::loadNextLevel()
{
	if(levelSet.isOver())
		return;
	QString level = levelSet.getCurrentLevel();
	PlayingMode * playingMode = static_cast<PlayingMode*>(gameMode);
	if(playingMode) {
		level = playingMode->getCurrentLevel();
	}
	gameMode = new FadeMode(level, true, this);
	connect(gameMode, SIGNAL(fadeIsEnded()), this, SLOT(showMessage()));
	connect(gameMode, SIGNAL(update()), this, SLOT(update()));
	update();
}

void SokobanWidget::showMessage()
{
	levelSet.moveToNextLevel();

	QString message = levelSet.isOver() ? tr("Levels are over.") : tr("Level: %1").arg(levelSet.getCurrentLevelName());
	gameMode = new MessageMode(!levelSet.isOver(), message, this);
	connect(gameMode, SIGNAL(messageIsEnded()), this, SLOT(startFadeIn()));
	update();
}

void SokobanWidget::startFadeIn()
{
	gameMode = new FadeMode(levelSet.getCurrentLevel(), false, this);
	connect(gameMode, SIGNAL(fadeIsEnded()), this, SLOT(startGame()));
	connect(gameMode, SIGNAL(update()), this, SLOT(update()));
	update();
}

void SokobanWidget::startGame()
{
	gameMode = new PlayingMode(levelSet.getCurrentLevel(), this);
	connect(gameMode, SIGNAL(levelIsSolved()), this, SLOT(loadNextLevel()));
	update();
}

void SokobanWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	gameMode->paint(&painter, rect());
}
