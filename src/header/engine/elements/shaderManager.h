#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/shader/shader.h>

namespace citrus::engine {
	class shaderManager : public element {
	private:
		std::map<std::string, graphics::shader*> shaders;

	public:
		graphics::shader* getShader(std::string name) {
			auto it = shaders.find(name);
			if(it != shaders.end()) return (*it).second;
			return nullptr;
		}

		void loadSimpleShader(std::string vertLoc, std::string fragLoc, std::string name) {
			if(getShader(name) == nullptr) {
				try {
					std::string vertSrc = util::loadEntireFile(vertLoc);
					std::string fragSrc = util::loadEntireFile(fragLoc);
					shaders[name] = new graphics::shader(vertSrc, fragSrc);
				} catch(const std::runtime_error& er) {
					e->Log("Failed to load simple shader \"" + name + "\": " + er.what());
				} catch(...) {
					e->Log("Failed to load simple shader \"" + name + "\": (Whack error)");
				}
			}
		}

		shaderManager(entityRef ent) : element(ent, typeid(shaderManager)) { }
		~shaderManager() {
			for(auto it : shaders) {
				delete it.second;
			}
		}
	};
}