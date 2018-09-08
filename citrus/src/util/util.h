#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <btBulletDynamicsCommon.h>
#include <glm\ext.hpp>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>
#include <util/json.h>

namespace citrus {
	namespace util {
		extern std::mutex _idMut;
		extern int _currentID;

		int nextID();

		std::vector<btVector3> glmToBtVector(const std::vector<glm::vec3>& verts);

		void spin_until(std::function<bool()> func, std::chrono::microseconds wait = std::chrono::microseconds(50));

		std::string keyString(int key, int scancode, int action, int mods);

		struct scopedProfiler {
			const std::string name;
			const std::chrono::high_resolution_clock::time_point start;
			scopedProfiler(const std::string& name);
			~scopedProfiler();
		};

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
	}
}

#endif