#pragma once

#include <vector>
#include <string>
#include <map>

#include <functional>
#include <mutex>
#include <chrono>
#include <experimental/filesystem>

#include <json.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <btBulletDynamicsCommon.h>
//#include <bullet/btBulletDynamicsCommon.h>

#define NO_COPY(classname)\
			private:\
				classname(const classname&) = delete;\
				classname& operator=(const classname&) = delete;\

namespace citrus {
	class transform;

	using nlohmann::json;

	using glm::quat;
	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using glm::ivec2;
	using glm::ivec3;
	using glm::ivec4;
	using glm::uvec2;
	using glm::uvec3;
	using glm::uvec4;
	using glm::mat3;
	using glm::mat4;

	using glm::translate;
	using glm::inverse;
	using glm::toMat4;
	using glm::toQuat;

	using path = std::experimental::filesystem::path;

	using std::string;
	using std::vector;
	using std::map;

	using std::mutex;
	using std::weak_ptr;
	using std::shared_ptr;
	using std::unique_ptr;

	namespace util {
		bool isPowerOfTwo(unsigned int val);

		string loadEntireFile(string path);

		extern mutex _soutMut;
		void sout(string text);

		bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, std::vector<unsigned char>& res)
		
		extern std::mutex _idMut;
		extern int _currentID;
		int nextID();

		string selectFolder();
		string selectFile(string filter = "");

		std::vector<btVector3> glmToBtVector(const std::vector<vec3>& verts);

		void spin_until(std::function<bool()> func, std::chrono::microseconds wait = std::chrono::microseconds(50));

		string keyString(int key, int scancode, int action, int mods);

		struct scopedProfiler {
			const string name;
			const std::chrono::high_resolution_clock::time_point start;
			scopedProfiler(const string& name);
			~scopedProfiler();
		};

		template<typename T> T rmod(T val, T mod);

		//value inside or outside of range [min, max) will modulo inside the range
		template<typename T> T wrap(T val, T min, T max);

		json save(vec2 vec);
		json save(vec3 vec);
		json save(vec4 vec);
		json save(quat q);
		vec2 loadVec2(json vec);
		vec3 loadVec3(json vec);
		vec4 loadVec4(json vec);
		quat loadQuat(json q);
		
		json save(transform trans);
		transform loadTransform(json trans);

		transform btToGlm(btTransform tr);
		btTransform glmToBt(transform tr);

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

		string toString(vec3 vec, int precision = 3);
		string toString(vec2 vec, int precision = 3);

		vec3 btToGlm(btVector3 vec);
		btVector3 glmToBt(vec3 vec);
		quat btToGlm(btQuaternion quat);
		btQuaternion glmToBt(quat quat);
	}
}

#include <citrus/dynamics/transform.h>
