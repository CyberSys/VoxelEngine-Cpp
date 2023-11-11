#include "engine.h"

#include <memory>
#include <iostream>
#include <glm/glm.hpp>

#include "audio/Audio.h"
#include "assets/Assets.h"
#include "assets/AssetsLoader.h"
#include "window/Window.h"
#include "window/Events.h"
#include "window/Camera.h"
#include "window/input.h"
#include "graphics/Batch2D.h"
#include "world/World.h"
#include "world/Level.h"
#include "voxels/Chunk.h"
#include "voxels/Chunks.h"
#include "voxels/ChunksController.h"
#include "voxels/ChunksStorage.h"
#include "objects/Player.h"
#include "frontend/world_render.h"
#include "frontend/hud.h"
#include "frontend/gui/GUI.h"

#include "coders/json.h"
#include "files/files.h"

using std::shared_ptr;
using glm::vec3;
using gui::GUI;

void load_settings(EngineSettings& settings, std::string filename) {
	std::string source = files::read_string(filename);
	std::unique_ptr<json::JObject> obj(json::parse(filename, source));
	obj->num("display-width", settings.displayWidth);
	obj->num("display-height", settings.displayHeight);
	obj->num("display-samples", settings.displaySamples);
	obj->num("display-swap-interval", settings.displaySwapInterval);
	obj->num("chunks-load-distance", settings.chunksLoadDistance);
	obj->num("chunks-load-speed", settings.chunksLoadSpeed);
	obj->num("chunks-padding", settings.chunksPadding);
	obj->num("fog-curve", settings.fogCurve);
}

void save_settings(EngineSettings& settings, std::string filename) {
	json::JObject obj;
	obj.put("display-width", settings.displayWidth);
	obj.put("display-height", settings.displayHeight);
	obj.put("display-samples", settings.displaySamples);
	obj.put("display-swap-interval", settings.displaySwapInterval);
	obj.put("chunks-load-distance", settings.chunksLoadDistance);
	obj.put("chunks-load-speed", settings.chunksLoadSpeed);
	obj.put("chunks-padding", settings.chunksPadding);
	obj.put("fog-curve", settings.fogCurve);
	files::write_string(filename, json::stringify(&obj, true, "  "));
}

Engine::Engine(const EngineSettings& settings) {
    this->settings = settings;
    
	Window::initialize(settings.displayWidth, 
	                   settings.displayHeight, 
	                   settings.displayTitle, 
	                   settings.displaySamples);
	Window::swapInterval(settings.displaySwapInterval);

	assets = new Assets();
	std::cout << "-- loading assets" << std::endl;
	AssetsLoader loader(assets);
	AssetsLoader::createDefaults(loader);
	AssetsLoader::addDefaults(loader);
	while (loader.hasNext()) {
		if (!loader.loadNext()) {
			delete assets;
			Window::terminate();
			throw initialize_error("could not to initialize assets");
		}
	}
	std::cout << "-- loading world" << std::endl;
	vec3 playerPosition = vec3(0, 64, 0);
	Camera* camera = new Camera(playerPosition, radians(90.0f));
	World* world = new World("world-1", "world/", 42);
	Player* player = new Player(playerPosition, 4.0f, camera);
	level = world->loadLevel(player, settings.chunksLoadDistance, settings.chunksPadding);

	std::cout << "-- initializing finished" << std::endl;

	Audio::initialize();

	gui = new GUI();
}

void Engine::updateTimers() {
	frame++;
	double currentTime = Window::time();
	delta = currentTime - lastTime;
	lastTime = currentTime;
}

void Engine::updateHotkeys() {
	if (Events::jpressed(keycode::O)) {
		occlusion = !occlusion;
	}
	if (Events::jpressed(keycode::F3)) {
		level->player->debug = !level->player->debug;
	}
	if (Events::jpressed(keycode::F5)) {
		for (uint i = 0; i < level->chunks->volume; i++) {
			shared_ptr<Chunk> chunk = level->chunks->chunks[i];
			if (chunk != nullptr && chunk->isReady()) {
				chunk->setModified(true);
			}
		}
	}
}

void Engine::mainloop() {
	Camera* camera = level->player->camera;
	std::cout << "-- preparing systems" << std::endl;
	WorldRenderer worldRenderer(level, assets);
	HudRenderer hud(gui, level, assets);
	Batch2D batch(1024);
	lastTime = Window::time();

	while (!Window::isShouldClose()){
		updateTimers();
		updateHotkeys();

		level->update(delta, Events::_cursor_locked, Events::_cursor_locked);
		level->chunksController->update(settings.chunksLoadSpeed);

		worldRenderer.draw(camera, occlusion, 1.6f / (float)settings.chunksLoadDistance, settings.fogCurve);
		hud.draw();
		if (level->player->debug) {
			hud.drawDebug( 1 / delta, occlusion);
		}
		gui->act();
		gui->draw(&batch, assets);

		Window::swapBuffers();
		Events::pullEvents();
	}
}

Engine::~Engine() {
	Audio::finalize();

	World* world = level->world;

	std::cout << "-- saving world" << std::endl;
	world->write(level);

	delete level;
	delete world;

	std::cout << "-- shutting down" << std::endl;
	delete assets;
	Window::terminate();
}