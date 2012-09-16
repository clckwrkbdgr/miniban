#include <QtGui/QImage>
#include "sokoban.h"
#include "sprites.h"
#include "xpm.h"

namespace XPMSprites { // XPM data for sprites.

/* XPM */
/* <width/cols> <height/rows> <colors> <char on pixel>*/
/* <Colors> */
/* <Pixels> */

static const char * FLOOR[] = {
	"8 8 1 2",
	"   c #151515",
	"                ",
	"                ",
	"                ",
	"                ",
	"                ",
	"                ",
	"                ",
	"                "
};
static const char * WALL[] = {
	"8 8 5 2",
	"   c #000000",
	"a  c #999999",
	"b  c #555555",
	"c  c #777777",
	"d  c #151515",
	"d a a a a a a d ",
	"b c c c c c c a ",
	"b c c c c c c a ",
	"b c c c c c c a ",
	"b c c c c c c a ",
	"b c c c c c c a ",
	"b c c c c c c a ",
	"d b b b b b b d "
};
static const char * EMPTY_SLOT[] = {
	"8 8 2 2",
	"   c #151515",
	"s  c #ff0000",
	"s s         s s ",
	"s             s ",
	"                ",
	"      s s       ",
	"      s s       ",
	"                ",
	"s             s ",
	"s s         s s "
};
static const char * PLAYER_ON_FLOOR[] = {
	"8 8 7 2",
	"   c #151515",
	"h  c #825106",
	"f  c #face8d",
	"t  c #66cc00",
	"a  c #000000",
	"b  c #666666",
	"p  c #448800",
	"  h h h h h     ",
	"  h f f f h     ",
	"  f a f a f     ",
	"    f f f       ",
	"t t t t t t t   ",
	"f t t t t t f   ",
	"f b a b a b f   ",
	"  p p   p p     "
};
static const char * PLAYER_ON_SLOT[] = {
	"8 8 8 2",
	"   c #151515",
	"s  c #ff0000",
	"h  c #825106",
	"f  c #face8d",
	"t  c #66cc00",
	"a  c #000000",
	"b  c #666666",
	"p  c #448800",
	"s h h h h h s s ",
	"s h f f f h   s ",
	"  f a f a f     ",
	"    f f f       ",
	"t t t t t t t   ",
	"f t t t t t f   ",
	"f b a b a b f s ",
	"s p p   p p s s "
};
static const char * BOX_ON_FLOOR[] = {
	"8 8 5 2",
	"   c #151515",
	"a  c #555555",
	"b  c #112288",
	"c  c #887888",
	"d  c #333333",
	"                ",
	"  c c b b c c   ",
	"  c d b b d c   ",
	"  b b a a b b   ",
	"  b b a a b b   ",
	"  c d b b d c   ",
	"  c c b b c c   ",
	"                "
};
static const char * BOX_ON_SLOT[] = {
	"8 8 6 2",
	"   c #151515",
	"s  c #00ff00",
	"a  c #555555",
	"b  c #112288",
	"c  c #887888",
	"d  c #333333",
	"s s         s s ",
	"s c c b b c c s ",
	"  c d b b d c   ",
	"  b b a a b b   ",
	"  b b a a b b   ",
	"  c d b b d c   ",
	"s c c b b c c s ",
	"s s         s s "
};

};

namespace Sprites { // Main.

QImage getSprite(int tileType)
{
	switch(tileType) {
		case Sokoban::TileType::FLOOR:           return XPM::toQImage(XPMSprites::FLOOR);
		case Sokoban::TileType::WALL:            return XPM::toQImage(XPMSprites::WALL);
		case Sokoban::TileType::EMPTY_SLOT:      return XPM::toQImage(XPMSprites::EMPTY_SLOT);
		case Sokoban::TileType::PLAYER_ON_FLOOR: return XPM::toQImage(XPMSprites::PLAYER_ON_FLOOR);
		case Sokoban::TileType::PLAYER_ON_SLOT:  return XPM::toQImage(XPMSprites::PLAYER_ON_SLOT);
		case Sokoban::TileType::BOX_ON_FLOOR:    return XPM::toQImage(XPMSprites::BOX_ON_FLOOR);
		case Sokoban::TileType::BOX_ON_SLOT:     return XPM::toQImage(XPMSprites::BOX_ON_SLOT);
		default: return QImage();
	}
}

};
