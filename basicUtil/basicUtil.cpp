#include "basicUtil.h"

#include <fstream>

namespace citrus {
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

	// forward declare operations for float and double
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
}