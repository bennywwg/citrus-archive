#include <filesystem>
#include <typeindex>

#include <nlohmann/json.hpp>

#include <engine/engine.h>
#include <graphics/window/window.h>
#include <graphics/framebuffer/framebuffer.h>


namespace citrus {
	namespace engine {
		using json = nlohmann::json;
		using clock = std::chrono::high_resolution_clock;

		void engine::_runRender() {
			util::sout("Render Thread Begin");
			_renderState.store(render_halted);

			try {
				_renderState.store(render_initializing);
				_win = nullptr;
				_win = new graphics::window(512, 512, "Citrus Engine");

				graphics::frameBuffer screen(_win);

				_renderState.store(render_doingRender);
				clock::time_point fpsSampleStart = clock::now();
				int fpsSample = 0;
				while(!stopped()) {
					//util::scopedProfiler fullFrame("Entire Frame");

					if((clock::now() - fpsSampleStart) >= std::chrono::seconds(1)) {
						fpsSampleStart = fpsSampleStart + std::chrono::seconds(1);
						_framesPerSecond = fpsSample;
						fpsSample = 0;
					}
					
					{
						//util::scopedProfiler profiler("Polling Input");
						_win->poll();
					}

					screen.clearAll(0.8f, 0.85f, 1.0f, 1.0f);

					man->flush();

					man->preRender();

					man->render();

					{
						//util::scopedProfiler profiler("Displaying Screen");
						_win->swapBuffers();
					}
					fpsSample++;
				}
			} catch(std::exception ex) {
				util::sout("Unrecoverable Error in Render Thread: " + std::string(ex.what()));
			} catch(...) {
				util::sout("Unrecoverable Error in Render Thread");
			}

			if(_win) delete _win;

			_renderState.store(render_finished);
			util::sout("Render Thread End");
		}

		graphics::window* engine::getWindow() {
			return _win;
		}
		
		engine::renderState engine::getRenderState() {
			return _renderState.load();
		}

		bool engine::getKey(graphics::windowInput::button but) {
			return _win->getKey(but);
		}

		void engine::loadLevel(std::path levelPath) {
			for(auto& it : std::experimental::filesystem::directory_iterator(levelPath)) {
				if(!std::experimental::filesystem::is_directory(it.path())) {
					std::string path = it.path().string();
					try {
						std::string read = util::loadEntireFile(path);
						json parsed = json::parse(read);
						std::string type = parsed["type"].get<std::string>();


					} catch(std::exception ex) {
						util::sout("Couldn't load resource " + path + ", " + ex.what() + "\n");
					}
				}
			}
		}

		void engine::setOrder(vector<std::type_index> order) {
			man->setOrder(order);
		}

		void engine::start() {
			_renderThread = std::thread(&engine::_runRender, this);
		}
		void engine::stop() {
			_closed = true;
		}
		bool engine::stopped() {
			return _closed;
		}

		engine::engine(double timeStep) : _timeStep(timeStep) {
			man = new manager(this);
			util::sout("Engine Start");
		}
		engine::~engine() {
			stop();
			_renderThread.join();
			util::sout("Engine Stop");
		}
	}
}