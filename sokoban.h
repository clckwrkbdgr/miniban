/*
# - wall
  - floor
@ - player on floor
. - empty slot
+ - man on slot
$ - box on floor
* - box on slot
movement - udrl for free moves and UDRL for pushes
*/
class Sokoban {
public:
	class InvalidPlayerCountException { public: int playerCount; };
	class InvalidControlException { public: QChar invalidControl; };
	class InvalidUndoException { public: QChar invalidUndoControl; };
	class OutOfMapException {};

	static QString process(const QString & field, const QChar & control) {
		Q_UNUSED(field);
		Q_UNUSED(control);
		return QString();
	}
	static QString undo(const QString & field, QString * history = NULL) {
		Q_UNUSED(field);
		Q_UNUSED(history);
		return QString();
	}
	static bool isSolved(const QString & field) {
		Q_UNUSED(field);
		return false;
	}
};
