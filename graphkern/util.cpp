#include <fstream>
#include <iostream>

#include <lodepng.h>

#include "util.h"

namespace citrus {
	using std::string;

	uint64_t roundUpAlign(uint64_t val, uint64_t align) {
		if (align == 0) return val;

		if (val % align == 0) return val;

		return (val / align + 1) * align;
	}
	uint64_t roundDownAlign(uint64_t val, uint64_t align) {
		if (align == 0) return val;

		if (val % align == 0) return val;

		return (val / align) * align;
	}

	template<typename T>
	T rmod(T val, T mod) {
		static_assert(std::is_floating_point<T>::value, "can only rmod a floating point type");
		return val - floor(val / mod) * mod;
	}
	template float rmod<float>(float val, float mod);
	template double rmod<double>(double val, double mod);

	template<typename T>
	T wrap(T val, T min, T max) {
		static_assert(std::is_floating_point<T>::value, "can only wrap a floating point type");
		T dif = max - min;
		T res = rmod(val - min, dif) + min;
		if (res < min || res >= max) return min;
		return res;
	}
	template float wrap<float>(float val, float min, float max);
	template double wrap<double>(double val, double min, double max);

	std::string loadEntireFile(std::string path) {
		std::ifstream f(path, std::ios::binary);
		if (!f.is_open()) throw std::runtime_error("Couldn't open file: " + path);
		std::string res;
		f.seekg(0, std::ios::end);
		res.reserve(size_t(f.tellg()));
		f.seekg(0, std::ios::beg);
		res.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
		return res;
	}
	void saveEntireFile(std::string path, std::string content) {
		std::ofstream(path, std::ofstream::out | std::ofstream::trunc) << content;
	}

	std::vector<fpath> filesInDirectory(fpath path, std::string suffix) {
		std::vector<fpath> res;
		if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
			for (auto it : std::filesystem::directory_iterator(path)) {
				if (!it.is_directory() && it.path().extension().string() == suffix) {
					res.push_back(it.path());
				}
			}
		}
		return res;
	}

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