#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace citrus {
	typedef std::filesystem::path fpath;

	uint64_t roundUpAlign(uint64_t val, uint64_t align);
	uint64_t roundDownAlign(uint64_t val, uint64_t align);

	template<typename T> T rmod(T val, T mod);
	template<typename T> T wrap(T val, T min, T max);

	std::vector<fpath> filesInDirectory(fpath path, std::string suffix);

	std::string loadEntireFile(std::string path);
	void saveEntireFile(std::string path, std::string content);
}