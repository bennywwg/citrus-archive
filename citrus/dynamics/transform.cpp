#include "citrus/dynamics/transform.h"

//To Do: Finish all the scaling stuff

namespace citrus {
	vec3 transform::getPosition() const {
		return _position;
	}
	void transform::setPosition(const vec3& position) {
		_position = position;
	}
	quat transform::getOrientation() const {
		return _orientation;
	}
	void transform::setOrientation(const quat& orientation) {
		_orientation = orientation;
	}
	vec3 transform::getScale() const {
		return _scale;
	}
	void transform::setScale(const vec3& scale) {
		_scale = scale;
	}
	transform transform::getTranslated(vec3 trans) {
		transform res = *this;
		res._position += trans;
		return res;
	}
	mat4 transform::getMat() const {
		return translate(_position) * toMat4(_orientation) * glm::scale(_scale);
	}
	mat4 transform::getMatNoScale() const {
		return translate(_position) * toMat4(_orientation);
	}
	transform transform::operator*(const transform & rhs) const {
		return getMat() * rhs.getMat();
	}
	bool transform::operator==(const transform & other) const {
		return _position == other._position && _orientation == other._orientation;
	}
	bool transform::operator!=(const transform & other) const {
		return !(*this == other);
	}
	transform::transform() : _position(0.0f, 0.0f, 0.0f), _orientation(1.0f, 0.0f, 0.0f, 0.0f), _scale(1.0f, 1.0f, 1.0f) { }
	transform::transform(const mat4& mat) : _position(glm::vec3(mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))), _orientation(glm::toQuat(mat)), _scale(1.0f, 1.0f, 1.0f) { }
	transform::transform(const vec3& position, const quat& orientation) : _position(position), _orientation(orientation), _scale(1.0f, 1.0f, 1.0f) { }
}