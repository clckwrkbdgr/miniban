#pragma once
#include <string>

struct Settings {
	int level_index;
	std::string levelset;
	Settings();
	void load();
	void save();
private:
	std::string filename;
};
