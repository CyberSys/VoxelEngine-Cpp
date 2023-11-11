#include <iostream>
#include <cmath>
#include <stdint.h>

#include <memory>
#include <filesystem>

#include "definitions.h"

#include "util/platform.h"
#include "engine.h"

int main() {
	platform::configure_encoding();
	setup_definitions();
	try {
	    EngineSettings settings;
	    settings.displayWidth = 1280;
	    settings.displayHeight = 720;
	    settings.displaySamples = 0; // верну мультисемплинг позже
	    settings.displayTitle = "VoxelEngine-Cpp v0.13";
	    settings.chunksLoadSpeed = 10;
	    settings.chunksLoadDistance = 12;
	    settings.chunksPadding = 2;
		settings.displaySwapInterval = 1;
		settings.fogCurve = 1.6f;

		std::string settings_file = platform::get_settings_file();
		if (std::filesystem::is_regular_file(settings_file)) {
			std::cout << "-- loading settings" << std::endl;
			load_settings(settings, settings_file);
		} else {
			std::cout << "-- creating settings file " << settings_file << std::endl;
			save_settings(settings, settings_file);
		}
		Engine engine(settings);
		engine.mainloop();
	}
	catch (const initialize_error& err) {
		std::cerr << "could not to initialize engine" << std::endl;
		std::cerr << err.what() << std::endl;
	}
	return 0;
}
