#include "util.h"
#include <fstream>

namespace citrus {
	using glm::translate;
	using glm::toMat4;
	using glm::scale;

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
	bool transform::operator==(const transform& other) const {
		return _position == other._position && _orientation == other._orientation;
	}
	bool transform::operator!=(const transform& other) const {
		return !(*this == other);
	}
	transform::transform() : _position(0.0f, 0.0f, 0.0f), _orientation(1.0f, 0.0f, 0.0f, 0.0f), _scale(1.0f, 1.0f, 1.0f) { }
	transform::transform(const mat4& mat) : _position(glm::vec3(mat* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))), _orientation(glm::toQuat(mat)), _scale(1.0f, 1.0f, 1.0f) { }
	transform::transform(const vec3& position, const quat& orientation) : _position(position), _orientation(orientation), _scale(1.0f, 1.0f, 1.0f) { }

	json save(vec2 vec) {
		return json({
			{"x", vec.x},
			{"y", vec.y}
			});
	}
	json save(vec3 vec) {
		return json({
			{"x", vec.x},
			{"y", vec.y},
			{"z", vec.z}
			});
	}
	json save(vec4 vec) {
		return json({
			{"x", vec.x},
			{"y", vec.y},
			{"z", vec.z},
			{"w", vec.w}
			});
	}
	json save(quat q) {
		return json({
			{"x", q.x},
			{"y", q.y},
			{"z", q.z},
			{"w", q.w}
			});
	}
	vec2 loadVec2(json vec) {
		return vec2(vec["x"].get<float>(), vec["y"].get<float>());
	}
	vec3 loadVec3(json vec) {
		return vec3(vec["x"].get<float>(), vec["y"].get<float>(), vec["z"].get<float>());
	}
	vec4 loadVec4(json vec) {
		return vec4(vec["x"].get<float>(), vec["y"].get<float>(), vec["z"].get<float>(), vec["w"].get<float>());
	}
	quat loadQuat(json q) {
		return quat(q["w"].get<float>(), q["x"].get<float>(), q["y"].get<float>(), q["z"].get<float>());
	}
	bool isVec2(json const& vec) {
		if (!vec.is_object()) return false;
		if (vec.find("x") == vec.end() || !vec["x"].is_number()) return false;
		if (vec.find("y") == vec.end() || !vec["y"].is_number()) return false;
		return true;
	}
	bool isVec3(json const& vec) {
		if (!vec.is_object()) return false;
		if (vec.find("x") == vec.end() || !vec["x"].is_number()) return false;
		if (vec.find("y") == vec.end() || !vec["y"].is_number()) return false;
		if (vec.find("z") == vec.end() || !vec["z"].is_number()) return false;
		return true;
	}
	bool isVec4(json const& vec) {
		if (!vec.is_object()) return false;
		if (vec.find("x") == vec.end() || !vec["x"].is_number()) return false;
		if (vec.find("y") == vec.end() || !vec["y"].is_number()) return false;
		if (vec.find("z") == vec.end() || !vec["z"].is_number()) return false;
		if (vec.find("w") == vec.end() || !vec["w"].is_number()) return false;
		return true;
	}
	bool isQuat(json const& q) {
		return isVec4(q);
	}

	std::vector<vec3> loadVec3Array(json v) {
		if (!v.is_array()) return { };
		json::array_t ar = v;
		std::vector<vec3> res;
		res.reserve(ar.size());
		for (int i = 0; i < ar.size(); i++) {
			res.push_back(loadVec3(v[i]));
		}
		return res;
	}
	std::vector<int> loadIntArray(json v) {
		if (!v.is_array()) return { };
		json::array_t ar = v;
		std::vector<int> res;
		res.reserve(ar.size());
		for (int i = 0; i < ar.size(); i++) {
			res.push_back(v[i].get<int>());
		}
		return res;
	}
	json save(std::vector<vec3> v) {
		json::array_t ar;
		for (int i = 0; i < v.size(); i++) {
			ar.push_back(save(v[i]));
		}
		return ar;
	}
	json save(std::vector<int> v) {
		json::array_t ar;
		for (int i = 0; i < v.size(); i++) {
			ar.push_back(v[i]);
		}
		return ar;
	}
	json save(transform trans) {
		return json({
			{"Position", save(trans.getPosition())},
			{"Orientation", save(trans.getOrientation())}
		});
	}
	transform loadTransform(json trans) {
		return transform(
			loadVec3(trans["Position"]),
			loadQuat(trans["Orientation"])
		);
	}
	bool isTransform(json const& trans) {
		if (!trans.is_object()) return false;
		if (trans.find("Position") == trans.end() || !isVec3(trans["Position"])) return false;
		if (trans.find("Orientation") == trans.end() || !isQuat(trans["Orientation"])) return false;
		return true;
	}

	std::string formatFloat(float f) {
		char buf[50];
		sprintf_s(buf, "%10.3f", f);
		return buf;
	}
}