#pragma once

#include <string>
#include <vector>
#include <filesystem>

#ifdef __linux__
#include <experimental/optional>
template<typename T>
using optional_t = std::experimental::optional<T>;
#elif _WIN32
#include <optional>
template<typename T>
using optional_t = std::optional<T>;
#endif

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <nlohmann/json.hpp>

namespace citrus {
	typedef std::filesystem::path fpath;

	using glm::quat;
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;
	using glm::ivec2;
	using glm::ivec3;
	using glm::ivec4;

	using nlohmann::json;

	class transform {
		vec3 _position;
		quat _orientation;
		vec3 _scale;

	public:
		vec3 getPosition() const;
		void setPosition(const vec3& position);

		quat getOrientation() const;
		void setOrientation(const quat& orientation);

		vec3 getScale() const;
		void setScale(const vec3& scale);

		transform getTranslated(vec3 trans);

		mat4 getMat() const;
		mat4 getMatNoScale() const;

		// direct memory comparison, no epsilon
		bool operator==(const transform& other) const;
		bool operator!=(const transform& other) const;

		transform();
		transform(const mat4& mat);
		transform(const vec3& position, const quat& orientation);
	};

	json save(vec2 vec);
	json save(vec3 vec);
	json save(vec4 vec);
	json save(quat q);
	vec2 loadVec2(json vec);
	vec3 loadVec3(json vec);
	vec4 loadVec4(json vec);
	quat loadQuat(json q);
	bool isVec2(json const& vec);
	bool isVec3(json const& vec);
	bool isVec4(json const& vec);
	bool isQuat(json const& q);

	std::vector<vec3> loadVec3Array(json v);
	std::vector<int> loadIntArray(json v);
	json save(std::vector<vec3> v);
	json save(std::vector<int> v);

	json save(transform trans);
	transform loadTransform(json trans);
	bool isTransform(json const& trans);

	template<class UnaryFunction>
	inline void recursive_iterate(json& j, UnaryFunction f) {
		for (auto it = j.begin(); it != j.end(); ++it) {
			if (it->is_structured()) {
				f(it);
				recursive_iterate(*it, f);
			} else {
				f(it);
			}
		}
	}

	std::string loadEntireFile(std::string path);
	void saveEntireFile(std::string path, std::string content);

	std::string formatFloat(float f);
}
