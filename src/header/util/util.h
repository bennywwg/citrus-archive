#pragma once

#ifndef UTIL_H
#define UTIL_H

//#include <bullet/btBulletDynamicsCommon.h>
#include <glm/ext.hpp>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>
#include <util/json.h>
#include <dynamics/transform.h>
#include <iomanip>
#include <sstream>

namespace citrus {
	namespace util {
		extern std::mutex _idMut;
		extern int _currentID;

		int nextID();

		//std::vector<btVector3> glmToBtVector(const std::vector<glm::vec3>& verts);

		void spin_until(std::function<bool()> func, std::chrono::microseconds wait = std::chrono::microseconds(50));

		std::string keyString(int key, int scancode, int action, int mods);

		struct scopedProfiler {
			const std::string name;
			const std::chrono::high_resolution_clock::time_point start;
			scopedProfiler(const std::string& name);
			~scopedProfiler();
		};

		inline json save(glm::vec2 vec) {
			return json({
				{"x", vec.x},
				{"y", vec.y}
			});
		}
		inline json save(glm::vec3 vec) {
			return json({
				{"x", vec.x},
				{"y", vec.y},
				{"z", vec.z}
			});
		}
		inline json save(glm::vec4 vec) {
			return json({
				{"x", vec.x},
				{"y", vec.y},
				{"z", vec.z},
				{"w", vec.w}
			});
		}
		inline json save(glm::quat q) {
			return json({
				{"x", q.x},
				{"y", q.y},
				{"z", q.z},
				{"w", q.w}
			});
		}
		inline glm::vec2 loadVec2(json vec) {
			return glm::vec2(vec["x"].get<float>(), vec["y"].get<float>());
		}
		inline glm::vec3 loadVec3(json vec) {
			return glm::vec3(vec["x"].get<float>(), vec["y"].get<float>(), vec["z"].get<float>());
		}
		inline glm::vec4 loadVec4(json vec) {
			return glm::vec4(vec["x"].get<float>(), vec["y"].get<float>(), vec["z"].get<float>(), vec["w"].get<float>());
		}
		inline glm::quat loadQuat(json q) {
			return glm::quat(q["w"].get<float>(), q["x"].get<float>(), q["y"].get<float>(), q["z"].get<float>());
		}
		
		inline json save(engine::transform trans) {
			return json({
				{"Position", save(trans.getPosition())},
				{"Orientation", save(trans.getOrientation())}
			});
		}
		inline engine::transform loadTransform(json trans) {
			return engine::transform(
				loadVec3(trans["Position"]),
				loadQuat(trans["Orientation"])
			);
		}

		template<class UnaryFunction>
		void recursive_iterate(json& j, UnaryFunction f) {
			for(auto it = j.begin(); it != j.end(); ++it) {
				if(it->is_structured()) {
					f(it);
					recursive_iterate(*it, f);
				} else {
					f(it);
				}
			}
		}

		inline std::string toString(glm::vec3 vec, int precision = 3) {
			std::stringstream ss;
			ss << std::fixed << std::setfill('0') << std::setw(8) << std::setprecision(3) << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">";
			return ss.str();
		}
	}
}

#endif
