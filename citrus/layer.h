#pragma once

#ifndef LAYER_H
#define LAYER_H

namespace citrus {
	namespace engine {
		class engine;

		class layer {
			private:
			engine* const _engine;
			friend class engine;

			public:
			virtual inline void onCreate() { }
			virtual inline void preRender() { }
			virtual inline void render() { }
			virtual inline void onDestroy() { }

			protected:
			layer(engine* eng) : _engine(eng) { }
		};
	}
}

#endif