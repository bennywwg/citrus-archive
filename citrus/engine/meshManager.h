#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/geometry/animesh.h>

namespace citrus::engine {
	class meshManager : public element {
		private:
		struct modelInfo {
			graphics::buffer* buf = nullptr;
			geom::animesh* mesh = nullptr;
			geom::animodel* model = nullptr;
			graphics::vertexArray* vao = nullptr;
		};

		std::vector<modelInfo> models;
		std::vector<geom::animation*> animations;
		
		public:
		geom::animesh& getMesh(int index) {
			if(index < 0) throw std::runtime_error("Mesh access index negative");
			if(index >= models.size() || !models[index].vao) throw std::runtime_error("Mesh at index does not exist");
			return *models[index].mesh;
		}
		graphics::vertexArray& getModel(int index) {
			if(index < 0) throw std::runtime_error("Model access index negative");
			if(index >= models.size() || !models[index].vao) throw std::runtime_error("Model at index does not exist");
			return *models[index].vao;
		}
		geom::animation& getAnimation(int index) {
			if(index < 0) throw std::runtime_error("Animation access index negative");
			if(index >= animations.size() || !animations[index]) throw std::runtime_error("Animation at index does not exist");
			return *animations[index];
		}

		void loadAnimation(std::string loc, int index) {
			if(index < 0) throw std::runtime_error("Animation create index negative");
			if(index >= animations.size()) animations.resize(index + 1);
			if(animations[index]) throw std::runtime_error("Animation already exists at index");

			geom::animation* ani = new	geom::animation();
			std::ifstream s(loc);
			ani->read(s);

			animations[index] = ani;
		}
		void loadMesh(std::string loc, int index) {
			util::sout("Started mesh " + std::to_string(index) + ": \"" + loc + "\"\n");
			if(index < 0) throw std::runtime_error("Model create index negative");
			if(index >= models.size()) models.resize(index + 1);
			if(models[index].vao) throw std::runtime_error("Model already exists at index");

			geom::animesh *mesh = new geom::animesh(loc, geom::xyz);
			if((*mesh).pos.size() == 0) {
				eng()->Log("Failed to load mesh " + std::to_string(index) + ": " + loc);
				return;
			}

			graphics::buffer* buf = new graphics::buffer((*mesh).requiredMemory());
			geom::animodel* model = new geom::animodel((*mesh), buf);
			graphics::vertexArray* vao = new graphics::vertexArray({
				graphics::vertexAttribute(0, false, *model->vertices),
				graphics::vertexAttribute(2, false, *model->uvs),
				graphics::vertexAttribute(3, false, *model->bones0),
				graphics::vertexAttribute(4, false, *model->bones1),
				graphics::vertexAttribute(5, false, *model->weights0),
				graphics::vertexAttribute(6, false, *model->weights1),
			}, *model->indices);

			models[index] = modelInfo {
				buf,
				mesh,
				model,
				vao
			};

			eng()->Log("Loaded mesh " + std::to_string(index) + ": \"" + loc + "\"");
			util::sout("Loaded mesh " + std::to_string(index) + ": \"" + loc + "\"\n");
		}
		void bindAllAvailableAnimations() {
			int successful = 0;
			for(int i = 0; i < models.size(); i++) {
				if(models[i].mesh == nullptr) continue;
				for(int j = 0; j < animations.size(); j++) {
					if(animations[j] == nullptr) continue;
					if(models[i].mesh->bindAnimation(*animations[j])) {
						successful++;
					}
				}
			}
			util::sout(std::to_string(successful) + " animations matched");
		}

		meshManager(entityRef ent) : element(ent, typeid(meshManager)) {
		}
		~meshManager() {
			for(auto it : models) {
				delete it.buf;
				delete it.model;
				delete it.vao;
			}
		}
	};
}