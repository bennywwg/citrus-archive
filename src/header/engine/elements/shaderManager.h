#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/shader/shader.h>

namespace citrus::engine {
	class shaderManager : public element {
		struct segmenter {
			
		};

	private:
		static constexpr int shaderCount = 32;

		graphics::shader *shaders[shaderCount];
		std::vector<eleRef<meshFilter>> filters[shaderCount];

		inline int allocateIndex() {
			for(int i = 0; i < shaderCount; i++) {
				if(shaders[i] == 0) {
					return i;
				}
			}
			throw std::runtime_error("Ran out of shader slots (" + std::to_string(shaderCount) + " max)");
		}
		inline void deallocateIndex(int i) {
			shaders[i] = nullptr;
		}

	public:
		

		inline void loadSimpleShader(std::string vertLoc, std::string fragLoc) {
			try {
				std::string vertSrc = util::loadEntireFile(vertLoc);
				std::string fragSrc = util::loadEntireFile(fragLoc);
				int index = allocateIndex();
				shaders[index] = new graphics::shader(vertSrc, fragSrc);
				e->Log("Loaded Shader \"" + vertLoc + "\" with index " + std::to_string(index));
			} catch(const std::runtime_error& er) {
				e->Log("Failed to load simple shader \"" + vertLoc + "\": " + er.what());
			} catch(...) {
				e->Log("Failed to load simple shader \"" + vertLoc + "\": (Whack error)");
			}
		}

		inline shaderManager(entityRef ent) : element(ent, typeid(shaderManager)) {
			memset(shaders, 0, sizeof(shaders));
		}
		inline ~shaderManager() {
			for(int i = 0; i < shaderCount; i++) {
				if(shaders[i]) delete shaders[i];
			}
		}
	};
}