#include <QtCore/QString>

namespace Sokoban {
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
