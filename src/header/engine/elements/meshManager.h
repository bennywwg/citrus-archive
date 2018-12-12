#pragma once

#include <engine/engine.h>
#include <engine/element.h>
#include <graphics/geometry/mesh.h>

namespace citrus::engine {
	class meshManager : public element {
		private:
		struct modelInfo {
			graphics::buffer* buf;
			geom::riggedModel* model;
			graphics::vertexArray* vao;
		};

		std::map<std::string, modelInfo> models;
		
		public:
		graphics::vertexArray* getModel(std::string name) {
			auto it = models.find(name);
			if(it != models.end()) return (*it).second.vao;
			return nullptr;
		}

		void loadMesh(std::string loc, std::string name) {
			if(getModel(name) == nullptr) {
				geom::conventionalmesh mesh(loc);
				if(mesh.pos.size() == 0) ent.eng()->Log("Failed to load mesh name \"" + name + "\" at: " + loc);

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

				models[name] = modelInfo {
					buf,
					model,
					vao
				};
			}
		}

		meshManager(entityRef ent) : element(ent, typeid(meshManager)) {
		}
		~meshManager() {
			for(auto it : models) {
				delete it.second.buf;
				delete it.second.model;
			}
		}
	};
}