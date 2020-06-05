#include <fstream>
#include <iostream>

#include <lodepng.h>

#include "util.h"
#include "../basicUtil/basicUtil.h"

namespace citrus {
	using std::string;

	// used with glsl row major formatting, compact packing
	void copyMat4x3ToRowMajor(mat4x3 const& m, float f[4 * 3]) {
		f[0] = m[0].x;
		f[1] = m[1].x;
		f[2] = m[2].x;
		f[3] = m[3].x;

		f[4] = m[0].y;
		f[5] = m[1].y;
		f[6] = m[2].y;
		f[7] = m[3].y;

		f[8] = m[0].z;
		f[9] = m[1].z;
		f[10] = m[2].z;
		f[11] = m[3].z;
	}

	bool loadPngImage(const char* name, int& outWidth, int& outHeight, bool& outHasAlpha, std::vector<unsigned char>& res) {
		std::vector<unsigned char> png;
		std::vector<unsigned char> image; //the raw pixels
		unsigned width = 0, height = 0;
		lodepng::State state; //optionally customize this one

		lodepng::load_file(png, name); //load the image file with given filename
		unsigned int error = lodepng::decode(image, width, height, state, png);

		//if there's an error, display it
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		res = image;
		outWidth = width;
		outHeight = height;
		outHasAlpha = state.info_raw.colortype == LCT_RGBA;

		return true;
	}

	void compileAllShaders(fpath shaderDir) {
		std::vector<fpath> vertPaths = filesInDirectory(shaderDir, ".vert");
		std::vector<fpath> fragPaths = filesInDirectory(shaderDir, ".frag");
		std::vector<fpath> allPaths; allPaths.reserve(vertPaths.size() + fragPaths.size());
		allPaths.insert(allPaths.end(), vertPaths.begin(), vertPaths.end());
		allPaths.insert(allPaths.end(), fragPaths.begin(), fragPaths.end());

		fpath compileDir = shaderDir / "build";

		if (!std::filesystem::exists(compileDir) || !std::filesystem::is_directory(compileDir)) {
			std::filesystem::create_directory(compileDir);
		}

		uint32_t numFailed = 0;
		uint32_t numSucceeded = 0;
		for (uint32_t i = 0; i < allPaths.size(); i++) {
			fpath compiled = compileDir / allPaths[i].filename();
			compiled += ".spv";

			if (!std::filesystem::exists(compiled) ||
				std::filesystem::last_write_time(compiled) < std::filesystem::last_write_time(allPaths[i])) {
				string compString = "glslangValidator.exe -V \"" + allPaths[i].string() + "\" -o \"" + compiled.string() + "\"";
				string res = execute(compString);
				if (res.length() >= 2) res.erase(res.length() - 2); //remove trailing endline
				if (res != allPaths[i].string()) {
					std::cout << "Failed to compile shader:\n" << res << "\n";
					numFailed++;
				} else {
					std::cout << "Compiled shader " << allPaths[i].filename().string() << "\n";
					numSucceeded++;
				}
			}
		}

		std::cout << "Shader info: " << std::to_string(numSucceeded) << " compiled, " << std::to_string(numFailed) << " failed, " << std::to_string(allPaths.size() - numFailed) << " up to date\n\n";
	}

	std::string execute(string const& command) {
		char tmpname[L_tmpnam_s];
		tmpnam_s(tmpname, L_tmpnam_s);
		std::string cmd = command + " >> " + tmpname;
		std::system(cmd.c_str());
		std::ifstream file(tmpname, std::ios::in | std::ios::binary);
		std::string result;
		if (file) {
			while (!file.eof()) result.push_back(file.get());
			file.close();
		}
		remove(tmpname);
		return result.substr(0, result.length() - 1);
	}

}