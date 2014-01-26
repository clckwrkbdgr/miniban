#include <QtDebug>
#include "playingmode.h"
#include "fademode.h"
#include <SDL2/SDL.h>

namespace { // Aux.

const int FADE_DELAY = 1; // msec
const int FADE_DELTA = 10;
const int FADE_COUNT = 200;

};

FadeMode::FadeMode(const Sokoban & levelToFade, const Sprites & _sprites, bool fadeOut, QObject * parent)
	: AbstractGameMode(parent), level(levelToFade), sprites(_sprites), isFadeOut(fadeOut), currentFade(isFadeOut ? FADE_COUNT : 0)
{
	timerId = startTimer(FADE_DELAY);
	invalidateRect();
}

FadeMode::~FadeMode()
{
	if(timerId) {
		killTimer(timerId);
	}
}

void FadeMode::invalidateRect()
{
	if(snapshot) {
		SDL_DestroyTexture(snapshot);
	}
	snapshot = 0;
}

void FadeMode::timerEvent(QTimerEvent*)
{
	bool isFadeEnded = (isFadeOut && currentFade <= 0) || (!isFadeOut && currentFade >= FADE_COUNT);
	if(isFadeEnded) {
		emit fadeIsEnded();
		killTimer(timerId);
		timerId = 0;
	}
	currentFade += isFadeOut ? -FADE_DELTA : FADE_DELTA;
	emit update();
}

void FadeMode::paint(SDL_Renderer * painter, const QRect & rect)
{
	if(snapshot == 0) {
		SDL_Surface * surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				rect.width(), rect.height(), 32,
				0x00ff0000,
				0x0000ff00,
				0x000000ff,
				0xff000000
				);
		SDL_Renderer * renderer = SDL_CreateSoftwareRenderer(surface);

		PlayingMode(level, sprites).paint(renderer, rect);
		snapshot = SDL_CreateTexture(painter, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, rect.width(), rect.height());
		SDL_UpdateTexture(snapshot, 0, surface->pixels, surface->pitch);
		SDL_SetTextureBlendMode(snapshot, SDL_BLENDMODE_BLEND);
	}

	SDL_RenderClear(painter);

	SDL_SetTextureAlphaMod(snapshot, currentFade * 255 / FADE_COUNT);
	SDL_RenderCopy(painter, snapshot, 0, 0);
}
