#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/geometry/mesh.h>

namespace citrus::engine {
	class meshManager : public element {
		private:
		struct modelInfo {
			graphics::buffer* buf = nullptr;
			geom::riggedModel* model = nullptr;
			graphics::vertexArray* vao = nullptr;
		};

		std::vector<modelInfo> models;
		
		public:
		graphics::vertexArray& getModel(int index) {
			if(index < 0) throw std::runtime_error("Model access index negative");
			if(index >= models.size() || !models[index].vao) throw std::runtime_error("Model at index does not exist");
			return *models[index].vao;
		}

		void loadMesh(std::string loc, int index) {
			if(index < 0) throw std::runtime_error("Model create index negative");
			if(index >= models.size()) models.resize(index + 1);
			if(models[index].vao) throw std::runtime_error("Model already exists at index");

			geom::conventionalmesh mesh(loc);
			if(mesh.pos.size() == 0) {
				e->Log("Failed to load mesh " + std::to_string(index) + ": " + loc);
				return;
			}

			graphics::buffer* buf = new graphics::buffer(mesh.requiredMemory());
			geom::riggedModel* model = new geom::riggedModel(mesh, buf);
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
				model,
				vao
			};

			e->Log("Loaded mesh " + std::to_string(index) + ": \"" + loc + "\"");
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