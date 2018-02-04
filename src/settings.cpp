#include "settings.h"
#include <chthon2/files.h>
#include <cstdlib>
#include <unistd.h>

std::string get_xdg_config_dir()
{
	static const char * xdg_config_dir = getenv("XDG_CONFIG_HOME");
	static std::string result = xdg_config_dir ? xdg_config_dir : "";
	if(result.empty()) {
		static const char * home_dir = getenv("HOME");
		result = std::string(home_dir ? home_dir : "") + "/.config";
	}
	return result;
}

std::string get_xdg_data_dir()
{
	static const char * xdg_data_dir = getenv("XDG_DATA_HOME");
	static std::string result = xdg_data_dir ? xdg_data_dir : "";
	if(result.empty()) {
		static const char * home_dir = getenv("HOME");
		result = std::string(home_dir ? home_dir : "") + "/.local/share";
	}
	return result;
}

std::string get_xdg_state_dir()
{
	static const char * xdg_state_dir = getenv("XDG_STATE_HOME");
	static std::string result = xdg_state_dir ? xdg_state_dir : "";
	if(result.empty()) {
		static const char * home_dir = getenv("HOME");
		result = std::string(home_dir ? home_dir : "") + "/.state";
	}
	return result;
}

Settings::Settings()
	: level_index(0)
{
	filename = get_xdg_state_dir() + "/miniban.save";
}

void Settings::load()
{
	if(Chthon::file_exists(filename)) {
		std::ifstream file(filename, std::ifstream::in);
		Chthon::Reader reader(file);
		reader.store(level_index).store(levelset);
	} else {
		static const std::vector<std::string> obsolete_config_files = {
			get_xdg_config_dir() + "/miniban.config",
			get_xdg_data_dir() + "/miniban.save",
		};
		for(const std::string & obsolete_config_file : obsolete_config_files) {
			if(!Chthon::file_exists(obsolete_config_file)) {
				continue;
			}
			std::ifstream file(obsolete_config_file, std::ifstream::in);
			Chthon::Reader reader(file);
			reader.store(level_index).store(levelset);
			// Immediately move to a proper location.
			save();
			unlink(obsolete_config_file.c_str());
			break;
		}
	}
}

void Settings::save()
{
	std::ofstream file(filename, std::ifstream::out);
	Chthon::Writer writer(file);
	writer.store(level_index).store(levelset);
}

