#include "transform.h"

namespace citrus {
	namespace engine {
		vec3 transform::getPosition() const {
			return _position;
		}
		void transform::setPosition(vec3 position) {
			_position = position;
		}
		quat transform::getOrientation() const {
			return _orientation;
		}
		void transform::setOrientation(quat orientation) {
			_orientation = orientation;
		}
		transform transform::getTranslated(vec3 trans) {
			transform res = *this;
			res._position += trans;
			return res;
		}
		mat4 transform::getMat() const {
			return translate(_position) * toMat4(_orientation);
		}
		transform::transform() { }
		transform::transform(vec3 position, quat orientation) : _position(position), _orientation(orientation) { }
	}
}


