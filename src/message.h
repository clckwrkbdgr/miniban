#pragma once
#include "counter.h"
#include <string>
#include <vector>
class Sprites;
class SDL_Renderer;
class SDL_Rect;

class Message {
public:
	Message(const Sprites & _sprites, const std::string & message_text);
	void set_text(const std::string & message_text);
	bool is_done() const;
	void processControl(int control);
	void paint(SDL_Renderer * painter, const SDL_Rect & rect);
	void processTime(int msec_passed);
private:
	const Sprites & sprites;
	bool done;
	std::vector<std::string> lines;
	unsigned max_width;
	Counter countdown;
};

