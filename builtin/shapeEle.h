#pragma once

#include "../mankern/manager.h"
#include "shape.h"

namespace citrus {
	class shapeEle : public element {
		shape _shape;
	public:
		inline void setShape(shape const& sha) {
			_shape = sha;
		}
		inline shape const& getShape() {
			return _shape;
		}

		inline void deserialize(json const& j) {
			_shape.type = (shapeType)j["type"].get<int>();
			_shape.state = loadVec3(j["state"]);
			if (j.find("ext") != j.end()) {
				string ext = j["ext"].get<string>();
				mesh m(ext);
				_shape.verts = m.pos;
				_shape.indices.reserve(m.index.size());
				for (int i = 0; i < m.index.size(); i++) {
					_shape.indices.push_back(m.index[i]);
				}
			} else {
				_shape.verts = loadVec3Array(j["verts"]);
				_shape.indices = loadIntArray(j["indices"]);
			}
		}
		inline json serialize() {
			return {
				{"type", (int)_shape.type },
				{"state", citrus::save(_shape.state) },
				{"verts", citrus::save(_shape.verts) },
				{"indices", citrus::save(_shape.indices) }
			};
		}
		inline uint64_t load(uint8_t* raw) {
			return 0;
		}
		inline grouping renderGUI() {
			grouping gp;
			/*gp.tr = ent().getGlobalTrans().getMat();
			gp.color = vec3(1.0f, 1.0f, 1.0f);
			if (_shape.type == shapeType::triangleMesh) {
				for (int i = 0; i < _shape.indices.size(); i += 3) {
					vec3 v0 = _shape.verts[_shape.indices[i]];
					vec3 v1 = _shape.verts[_shape.indices[i + 1]];
					vec3 v2 = _shape.verts[_shape.indices[i + 2]];
					gp.data.push_back(v0);
					gp.data.push_back(v1);
					gp.data.push_back(v2);
					gp.barycentricdata.push_back(vec3(1.0f, 0.0f, 0.0f));
					gp.barycentricdata.push_back(vec3(0.0f, 1.0f, 0.0f));
					gp.barycentricdata.push_back(vec3(0.0f, 0.0f, 1.0f));
				}
				gp.uvdata.resize(gp.data.size(), vec2(0.0f, 0.0f));
			} else if (_shape.type == shapeType::sphere) {
				gp.addSphere(_shape.state.x, 4, 8);
			}*/
			return gp;
		}
		inline shapeEle(entRef const& ent, manager & man, void *usr) : element(ent, man, usr, typeid(shapeEle)) {
		}
	};
}