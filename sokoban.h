#include <QtCore/QString>

class Sokoban {
public:
	class InvalidPlayerCountException {
		public:
			InvalidPlayerCountException(int wrongPlayerCount) : playerCount(wrongPlayerCount) {}
			int playerCount;
	};
	class InvalidControlException {
		public:
			InvalidControlException(int control) : invalidControl(control) {}
			QChar invalidControl;
	};
	class InvalidUndoException {
		public:
			InvalidUndoException(int control) : invalidUndoControl(control) {}
			QChar invalidUndoControl;
	};
	class OutOfMapException {};

	static QString process(const QString & field, const QChar & control);
	static QString undo(const QString & field, QString * history = NULL);
	static bool isSolved(const QString & field);
};
