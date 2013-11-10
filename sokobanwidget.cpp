#include <QtDebug>
#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include "playingmode.h"
#include "messagemode.h"
#include "fademode.h"
#include "sokobanwidget.h"

namespace {

QMap<int, QString> generateKeyToTextMap()
{
	QMap<int, QString> result;
	result[Qt::Key_0]         = "0";
	result[Qt::Key_1]         = "1";
	result[Qt::Key_Backspace] = "Backspace";
	result[Qt::Key_Down]      = "Down";
	result[Qt::Key_H]         = "H";
	result[Qt::Key_Home]      = "Home";
	result[Qt::Key_J]         = "J";
	result[Qt::Key_K]         = "K";
	result[Qt::Key_L]         = "L";
	result[Qt::Key_Y]         = "Y";
	result[Qt::Key_U]         = "U";
	result[Qt::Key_B]         = "B";
	result[Qt::Key_N]         = "N";
	result[Qt::Key_Left]      = "Left";
	result[Qt::Key_O]         = "O";
	result[Qt::Key_Q]         = "Q";
	result[Qt::Key_R]         = "R";
	result[Qt::Key_Right]     = "Right";
	result[Qt::Key_Space]     = "Space";
	result[Qt::Key_U]         = "U";
	result[Qt::Key_Up]        = "Up";
	result[Qt::Key_Z]         = "Z";
	result[Qt::Key_X]         = "X";
	result[Qt::Key_Period]    = ".";
	return result;
}

QMap<QString, int> generateTextToControlMap()
{
	QMap<QString, int> result;
	result["Left"]  = result["H"] = AbstractGameMode::CONTROL_LEFT;
	result["Down"]  = result["J"] = AbstractGameMode::CONTROL_DOWN;
	result["Up"]    = result["K"] = AbstractGameMode::CONTROL_UP;
	result["Right"] = result["L"] = AbstractGameMode::CONTROL_RIGHT;
	result["Y"] = AbstractGameMode::CONTROL_UP_LEFT;
	result["U"] = AbstractGameMode::CONTROL_UP_RIGHT;
	result["B"] = AbstractGameMode::CONTROL_DOWN_LEFT;
	result["N"] = AbstractGameMode::CONTROL_DOWN_RIGHT;
	result["Shift-Left"]  = result["Shift-H"] = AbstractGameMode::CONTROL_RUN_LEFT;
	result["Shift-Down"]  = result["Shift-J"] = AbstractGameMode::CONTROL_RUN_DOWN;
	result["Shift-Up"]    = result["Shift-K"] = AbstractGameMode::CONTROL_RUN_UP;
	result["Shift-Right"] = result["Shift-L"] = AbstractGameMode::CONTROL_RUN_RIGHT;
	result["X"]         = AbstractGameMode::CONTROL_TARGET;
	result["."]         = AbstractGameMode::CONTROL_GOTO;
	result["Space"]     = AbstractGameMode::CONTROL_SKIP;
	result["Ctrl-Z"]    = AbstractGameMode::CONTROL_UNDO;
	result["Backspace"] = AbstractGameMode::CONTROL_UNDO;
	result["Ctrl-R"]    = AbstractGameMode::CONTROL_HOME;
	result["Home"]      = AbstractGameMode::CONTROL_HOME;
	result["O"]         = AbstractGameMode::CONTROL_OPEN;
	result["Ctrl-O"]    = AbstractGameMode::CONTROL_OPEN;
	result["Ctrl-Q"]    = AbstractGameMode::CONTROL_QUIT;
	result["Q"]         = AbstractGameMode::CONTROL_QUIT;

	result["Ctrl-0"]    = AbstractGameMode::CONTROL_CHEAT_RESTART;
	result["Ctrl-1"]    = AbstractGameMode::CONTROL_CHEAT_SKIP_LEVEL;
	return result;
}

const QMap<int, QString> keyToText = generateKeyToTextMap();
const QMap<QString, int> textToControl = generateTextToControlMap();

}

SokobanWidget::SokobanWidget(QWidget * parent)
	: QWidget(parent), gameMode(0), sprites("sokoban.png")
{
	QStringList args = QCoreApplication::arguments();
	args.removeAt(0);
	QString commandLineFilename = args.value(0);

	QSettings settings;
	int lastLevelIndex = settings.value("levels/lastindex", 0).toInt();
	QString lastLevelSet = settings.value("levels/levelset", QString()).toString();
	if(!lastLevelSet.isEmpty()) {
		if(commandLineFilename == lastLevelSet) {
			levelSet = LevelSet(lastLevelSet, lastLevelIndex);
		} else {
			levelSet = LevelSet(commandLineFilename, 0);
		}
	}

	showInterlevelMessage();
}

SokobanWidget::~SokobanWidget()
{
	QSettings settings;
	settings.setValue("levels/lastindex", levelSet.getCurrentLevelIndex());
	settings.setValue("levels/levelset", levelSet.getCurrentLevelSet());
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

	QString pressedCombination = keyToText[event->key()];
	if(isShiftDown) {
		pressedCombination.prepend("Shift-");
	}
	if(isCtrlDown) {
		pressedCombination.prepend("Ctrl-");
	}

	int control = textToControl.value(pressedCombination);
	gameMode->processControl(control);

	switch(control) {
		case AbstractGameMode::CONTROL_CHEAT_SKIP_LEVEL:
			if(playingMode) {
				loadNextLevel();
			}
			break;
		case AbstractGameMode::CONTROL_QUIT: close(); break;
		case AbstractGameMode::CONTROL_OPEN: openLevelSet(); break;
		default: QWidget::keyPressEvent(event); break;
	}
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
	levelSet.moveToNextLevel();

	gameMode = new FadeMode(level, sprites, true, this);
	connect(gameMode, SIGNAL(fadeIsEnded()), this, SLOT(showInterlevelMessage()));
	connect(gameMode, SIGNAL(update()), this, SLOT(update()));
	update();
}

void SokobanWidget::showInterlevelMessage()
{
	QString levelName = tr("%1: %4\n%2/%3")
		.arg(levelSet.getLevelSetTitle())
		.arg(levelSet.getCurrentLevelIndex() + 1)
		.arg(levelSet.getLevelCount())
		.arg(levelSet.getCurrentLevelName())
		;
	QString message = levelSet.isOver() ? tr("%1\nLevels are over.").arg(levelSet.getLevelSetTitle()) : levelName;
	gameMode = new MessageMode(!levelSet.isOver(), message, this);
	connect(gameMode, SIGNAL(messageIsEnded()), this, SLOT(startFadeIn()));
	update();
}

void SokobanWidget::startFadeIn()
{
	gameMode = new FadeMode(levelSet.getCurrentLevel(), sprites, false, this);
	connect(gameMode, SIGNAL(fadeIsEnded()), this, SLOT(startGame()));
	connect(gameMode, SIGNAL(update()), this, SLOT(update()));
	update();
}

void SokobanWidget::startGame()
{
	gameMode = new PlayingMode(levelSet.getCurrentLevel(), sprites, this);
	connect(gameMode, SIGNAL(levelIsSolved()), this, SLOT(loadNextLevel()));
	update();
}

void SokobanWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	gameMode->paint(&painter, rect());
}
