#include "settings.h"
#include <chthon2/files.h>
#include <cstdlib>

Settings::Settings()
	: level_index(0)
{
	const char * home_dir = getenv("HOME");
	filename = std::string(home_dir ? home_dir : "") + "/.config/miniban.config";
}

void Settings::load()
{
	if(Chthon::file_exists(filename)) {
		std::ifstream file(filename, std::ifstream::in);
		Chthon::Reader reader(file);
		reader.store(level_index).store(levelset);
	}
}

void Settings::save()
{
	std::ofstream file(filename, std::ifstream::out);
	Chthon::Writer writer(file);
	writer.store(level_index).store(levelset);
}

