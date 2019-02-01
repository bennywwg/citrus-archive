#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <atomic>
#include <typeindex>
#include <thread>
#include <citrus/util.h>
#include <citrus/engine/manager.h>
#include <citrus/graphics/window.h>
#include <memory>
#include <citrus/graphics/tilemapFont.h>
#include <citrus/graphics/camera.h>


namespace citrus {
	namespace graphics {
		class window;
		class shader;
		class texture;
	}

	namespace geom {
		class model;
	}

	namespace dynamics {
		class world;
	}

	namespace editor {
		class ctEditor;
	}

	namespace engine {
		class manager;

		using std::vector;
		using std::string;
		using std::pair;
		using std::tuple;
		using clock = std::chrono::high_resolution_clock;

		class engine {
			private:
			clock::time_point _engineStart;
			std::mutex _logMut;
			vector<pair<double, string>> _log;
			public:
			editor::ctEditor* ed;

			void Log(string str);
			void Log(std::vector<string> strs);
			vector<pair<double, string>> flushLog();
			private:
			graphics::window* _win;

			std::atomic_bool _closed;
			const double _timeStep;

			std::thread _renderThread;

			public:
			int frame = 0;

			manager* man;

			private:
			int _framesPerSecond = 0;

			public: enum renderState {
				render_doingRender,
				render_initializing,
				render_halted,
				render_finished
			};
			private: std::atomic<renderState> _renderState;
			private: void _runRender();


			public:

			int fps();
			renderState getRenderState();

			double time();
			double dt();
			
			graphics::window* getWindow();

			template<typename T>
			inline std::vector<eleRef<T>> getAllOfType() {
				return man->ofType<T>();
			}

			float controllerValue(graphics::windowInput::analog a);
			bool controllerButton(graphics::windowInput::button b);
			bool getKey(graphics::windowInput::button but);

			void setOrder(std::vector<std::type_index> order);

			void loadLevel(std::path levelPath);

			//lifetiem events
			void start();
			void stop();
			bool stopped();

			engine(double timeStep);
			~engine();
			
			NO_COPY(engine)
		};
	}
}

#endif
