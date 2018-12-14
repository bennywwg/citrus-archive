#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/texture/colorTexture.h>
#include <optional>

namespace citrus::engine {
	class textureManager : public element {
		private:
		std::vector<graphics::texture3b*> textures;

		public:
		graphics::texture3b& getTexture(int index) {
			if(index < 0) throw std::runtime_error("Texture access index negative");
			if(index >= textures.size() || !textures[index]) throw std::runtime_error("Texture at index does not exist");
			return *textures[index];
		}

		void loadPNG(std::string loc, int index) {
			if(index < 0) throw std::runtime_error("Texture creation index negative");
			if(index >= textures.size()) textures.resize(index + 1, nullptr);
			if(textures[index]) throw std::runtime_error("Texture alread exists at index " + std::to_string(index));
			try {
				textures[index] = new graphics::texture3b(graphics::image3b(loc));
				e->Log("Loaded texture " + std::to_string(index) + ": \"" + loc + "\"");
			} catch(const std::runtime_error& er) {
				e->Log("Failed to load texture " + std::to_string(index) + " \"" + loc + "\": " + er.what());
			} catch(...) {
				e->Log("Failed to load texture " + std::to_string(index) + " \"" + loc + "\": (Whack error)");
			}
		}

		textureManager(entityRef ent) : element(ent, typeid(textureManager)) { }
		~textureManager() {
			for(auto it : textures) {
				if(it) delete it;
			}
		}
	};
}