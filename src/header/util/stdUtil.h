#pragma once

#ifndef STDUTIL_H
#define STDUTIL_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <mutex>
#include <fstream>
#include <functional>
#include <filesystem>

using std::string;
using std::vector;
using std::map;

using std::mutex;
using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;

namespace std {
	using path = experimental::filesystem::path;
}

#define NO_COPY(classname)\
			private:\
				classname(const classname&) = delete;\
				classname& operator=(const classname&) = delete;\

namespace citrus {
	namespace util {
		bool isPowerOfTwo(unsigned int val);

		string loadEntireFile(string path);

		extern mutex _soutMut;
		void sout(string text);
	}
}

#endif