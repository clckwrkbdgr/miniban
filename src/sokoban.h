#include <QtCore/QString>

namespace Sokoban {

namespace TileType { enum TileTypes {
	FLOOR           = ' ',
	WALL            = '#',
	PLAYER_ON_FLOOR = '@',
	EMPTY_SLOT      = '.',
	PLAYER_ON_SLOT  = '+',
	BOX_ON_FLOOR    = '$',
	BOX_ON_SLOT     = '*'
}; };

namespace Control { enum Controls {
	UP         = 'u',
	DOWN       = 'd',
	LEFT       = 'l',
	RIGHT      = 'r',
	PUSH_UP    = 'U',
	PUSH_DOWN  = 'D',
	PUSH_LEFT  = 'L',
	PUSH_RIGHT = 'R'
}; };

class InvalidPlayerCountException {
	public:
		InvalidPlayerCountException(int wrongPlayerCount) : playerCount(wrongPlayerCount) {}
		int playerCount;
};
class InvalidControlException {
	public:
		InvalidControlException(const QChar & control) : invalidControl(control) {}
		QChar invalidControl;
};
class InvalidUndoException {
	public:
		InvalidUndoException(const QChar & control) : invalidUndoControl(control) {}
		QChar invalidUndoControl;
};
class OutOfMapException {};

QString process(const QString & field, const QChar & control);
QString undo(const QString & field, QString * history = NULL);
bool isSolved(const QString & field);

};
