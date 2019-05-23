#include "message.h"
#include "playingmode.h"
#include <SDL2/SDL.h>

void Message::processTime(int msec_passed)
{
	if(!done) {
		countdown.tick(msec_passed);
		if(!countdown.is_active()) {
			done = true;
		}
	}
}

Message::Message(const Sprites & _sprites, const std::string & message_text)
	: sprites(_sprites), done(false), countdown(1000)
{
	set_text(message_text);
}

void Message::set_text(const std::string & message_text)
{
	countdown.start();
	done = false;
	lines.clear();
	lines.push_back(std::string());
	max_width = 0;
	for(char ch : message_text) {
		if(ch != '\n') {
			lines.back() += ch;
		} else {
			max_width = std::max(max_width, (unsigned)lines.back().size());
			lines.push_back(std::string());
		}
	}
	max_width = std::max(max_width, (unsigned)lines.back().size());
}

bool Message::is_done() const
{
	return done;
}

void Message::processControl(int control)
{
	if(control == Game::CONTROL_SKIP) {
		done = true;
	}
}

void center_rect(SDL_Rect & rect, const SDL_Rect & other_rect)
{
	int x = other_rect.x + other_rect.w / 2;
	int y = other_rect.y + other_rect.h / 2;
	rect.x = x - rect.w / 2;
	rect.y = y - rect.h / 2;
}

void Message::paint(SDL_Renderer * painter, const SDL_Rect & rect)
{
	SDL_RenderClear(painter);
	SDL_Rect dest_rect;
	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.w = sprites.getCharRect(0).w;
	dest_rect.h = sprites.getCharRect(0).h;

	SDL_Rect text_rect;
	text_rect.x = 0;
	text_rect.y = 0;
	text_rect.w = dest_rect.w * max_width;
	text_rect.h = dest_rect.h * lines.size();

	center_rect(text_rect, rect);
	dest_rect.y = text_rect.y;
	for(const std::string & line : lines) {
		dest_rect.x = text_rect.x + (dest_rect.w * (max_width - line.size())) / 2 ;
		for(char ch : line) {
			SDL_Rect char_rect = sprites.getCharRect(ch);
			SDL_RenderCopy(painter, sprites.getFont(), &char_rect, &dest_rect);
			dest_rect.x += dest_rect.w;
		}
		dest_rect.y += dest_rect.h;
	}
}

