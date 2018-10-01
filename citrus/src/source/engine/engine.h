#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <atomic>
#include <typeindex>
#include <util/stdUtil.h>
#include <engine/manager.h>
#include <graphics/window/window.h>


namespace citrus {
	namespace graphics {
		class window;
		class shader;
		class texture;
	}

	namespace geom {
		class simpleModel;
	}

	namespace dynamics {
		class world;
	}

	namespace engine {
		class manager;

		class engine {
			private:
			graphics::window* _win;

			std::atomic_bool _closed = false;
			const double _timeStep;

			std::thread _renderThread;

			public:
			manager* man;

			private:
			int _framesPerSecond = 0;

			public: enum renderState {
				render_doingRender,
				render_initializing,
				render_halted,
				render_finished
			};
			private: std::atomic<renderState> _renderState = render_halted;
			private: void _runRender();

			public:
			renderState getRenderState();

			graphics::window* getWindow();

			template<typename T>
			inline element* getAllOfType() {
				return man->ofType<T>();
			}

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