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
			max_width = std::max(max_width, lines.back().size());
			lines.push_back(std::string());
		}
	}
	max_width = std::max(max_width, lines.back().size());
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

void Message::paint(SDL_Renderer * painter, const QRect & rect)
{
	SDL_RenderClear(painter);
	SDL_Rect dest_rect;
	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.w = sprites.getCharRect(0).width();
	dest_rect.h = sprites.getCharRect(0).height();

	QRect text_rect = QRect(0, 0, dest_rect.w * max_width, dest_rect.h * lines.size());
	text_rect.moveCenter(rect.center());
	dest_rect.y = text_rect.y();
	for(const std::string & line : lines) {
		dest_rect.x = text_rect.x() + (dest_rect.w * (max_width - line.size())) / 2 ;
		for(char ch : line) {
			QRect char_qrect = sprites.getCharRect(ch);
			SDL_Rect char_rect;
			char_rect.x = char_qrect.x();
			char_rect.y = char_qrect.y();
			char_rect.w = char_qrect.width();
			char_rect.h = char_qrect.height();

			SDL_RenderCopy(painter, sprites.getFont(), &char_rect, &dest_rect);
			dest_rect.x += dest_rect.w;
		}
		dest_rect.y += dest_rect.h;
	}
}

