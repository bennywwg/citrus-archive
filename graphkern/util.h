#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <filesystem>

namespace citrus {
	typedef std::filesystem::path fpath;

	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using glm::quat;
	using glm::mat3;
	using glm::mat4;
	using glm::mat4x3;

	using glm::ivec2;
	using glm::uvec4;

	using std::string;

	using hpclock = std::chrono::high_resolution_clock;
	using hptime = hpclock::time_point;

	void copyMat4x3ToRowMajor(mat4x3 const& m, float f[4 * 3]);

	bool loadPngImage(const char* name, int& outWidth, int& outHeight, bool& outHasAlpha, std::vector<unsigned char>& res);

	void compileAllShaders(fpath shaderDir);

	std::string execute(string const& command);
}