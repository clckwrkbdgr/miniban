#include <QtCore/QString>

class Sokoban {
public:
	enum Cell {
		FLOOR           = ' ',
		WALL            = '#',
		PLAYER_ON_FLOOR = '@',
		EMPTY_SLOT      = '.',
		PLAYER_ON_SLOT  = '+',
		BOX_ON_FLOOR    = '$',
		BOX_ON_SLOT     = '*'
	};
	enum Control {
		NONE       = 0,
		UP         = 'u',
		DOWN       = 'd',
		LEFT       = 'l',
		RIGHT      = 'r',
		PUSH_UP    = 'U',
		PUSH_DOWN  = 'D',
		PUSH_LEFT  = 'L',
		PUSH_RIGHT = 'R'
	};

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

	Sokoban(const QString & levelField, const QString & backgroundHistory = QString());
	void processControls(const QString & controls);
	QString toString() const;
	QString historyAsString() const;
	bool undo();
	bool isSolved();
};
