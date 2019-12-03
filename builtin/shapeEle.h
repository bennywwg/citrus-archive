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
		}
		inline uint64_t load(uint8_t* raw) {
			return 0;
		}
		inline shapeEle(entRef const& ent, manager & man, void *usr) : element(ent, man, usr, typeid(shapeEle)) {
		}
	};
}