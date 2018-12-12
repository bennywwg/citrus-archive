#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/texture/colorTexture.h>

namespace citrus::engine {
	class textureManager : public element {
		private:
		std::map<std::string, graphics::texture3b*> textures;

		public:
		graphics::texture3b* getTexture(std::string name) {
			auto it = textures.find(name);
			if(it != textures.end()) return (*it).second;
			return nullptr;
		}

		void loadPNG(std::string loc, std::string name) {
			if(getTexture(name) == nullptr) {
				try {
					textures[name] = new graphics::texture3b(graphics::image3b(loc));
				} catch(const std::runtime_error& er) {
					e->Log("Failed to load texture \"" + name + "\": " + er.what());
				} catch(...) {
					e->Log("Failed to load texture \"" + name + "\": (Whack error)");
				}
			}
		}

		textureManager(entityRef ent) : element(ent, typeid(textureManager)) { }
		~textureManager() {
			for(auto it : textures) {
				delete it.second;
			}
		}
	};
}