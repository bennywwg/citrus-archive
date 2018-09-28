#include <filesystem>
#include <typeindex>

#include <nlohmann/json.hpp>

#include <engine/engine.h>
#include <graphics/window/window.h>
#include <graphics/framebuffer/framebuffer.h>


namespace citrus {
	namespace engine {
		using json = nlohmann::json;

		void engine::Log(string str) {
			std::lock_guard<std::mutex> lock(_logMut);
			_log.emplace_back(this->time(), str);
		}

		vector<pair<double, string>> engine::flushLog() {
			std::lock_guard<std::mutex> lock(_logMut);
			auto res = _log;
			_log.clear();
			return res;
		}

		void engine::_runRender() {
			Log("Render Thread Begin");
			_renderState.store(render_halted);

			try {
				_renderState.store(render_initializing);
				_win = nullptr;
				_win = new graphics::window(512, 512, "Citrus Engine");

				this->Log(_win->getAdapter());

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
				Log("Unrecoverable Error in Render Thread: " + std::string(ex.what()));
			} catch(...) {
				Log("Unrecoverable Error in Render Thread");
			}

			if(_win) delete _win;

			_renderState.store(render_finished);
			Log("Render Thread End");
		}

		graphics::window* engine::getWindow() {
			return _win;
		}
		
		engine::renderState engine::getRenderState() {
			return _renderState.load();
		}

		double engine::time() {
			return (clock::now() - _engineStart).count() * 0.000000001;
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
						Log("Couldn't load resource " + path + ", " + ex.what() + "\n");
					}
				}
			}
		}

		void engine::setOrder(vector<std::type_index> order) {
			man->setOrder(order);
		}

		void engine::start() {
			_engineStart = clock::now();
			_renderThread = std::thread(&engine::_runRender, this);
		}
		void engine::stop() {
			_closed = true;
		}
		bool engine::stopped() {
			return _closed;
		}

		engine::engine(double timeStep) : _timeStep(timeStep) {
			_engineStart = clock::now();
			man = new manager(this);
			Log("Engine Start");
		}
		engine::~engine() {
			stop();
			_renderThread.join();
			Log("Engine Stop");
		}
	}
}