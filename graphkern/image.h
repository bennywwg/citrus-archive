#pragma once

#include "util.h"

namespace citrus {
#pragma pack(push, 1)
	template<typename V>
	struct pixel1 {
		V r;
	};
	template<typename V>
	struct pixel2 {
		V r, g;
	};
	template<typename V>
	struct pixel3 {
		V r, g, b;
	};
	template<typename V>
	struct pixel4 {
		V r, g, b, a;
	};
#pragma pack(pop)

	template<typename P>
	class imageT {
		using T = decltype(((P*)nullptr)->r);
		const uint32_t N = sizeof(P) / sizeof(T);

	private:
		int _width, _height;
		std::vector<P> _userData;

		void exceptIfOutOfBounds(unsigned int x, unsigned int y) const {
			if (x >= _width || y >= _height) throw std::runtime_error("Image access out of bounds");
		}

	public:
		const P* data() const {
			return _userData.data();
		}
		P* data() {
			return _userData.data();
		}

		size_t size() const {
			return _userData.size() * sizeof(P);
		}
		size_t pixelCount() const {
			return _userData.size();
		}

		unsigned int width() const {
			return _width;
		}
		unsigned int height() const {
			return _height;
		}

		const P& at(unsigned int x, unsigned int y) const {
			exceptIfOutOfBounds(x, y);

			return _userData[y * _width + x];
		}
		P& at(unsigned int x, unsigned int y) {
			exceptIfOutOfBounds(x, y);

			return _userData[y * _width + x];
		}

		unsigned int getNumComponents() const {
			return sizeof(P) / sizeof(T);
		}


		imageT() {
			_width = 0;
			_height = 0;
		}

		//black image
		imageT(unsigned int width, unsigned int height) : _width(width), _height(height) {
			if (width == 0 || height == 0) throw std::runtime_error("Width and Height of image must be nonzero");

			_userData.resize(width * height);
			memset(_userData.data(), 0, _userData.size() * sizeof(P)); //zero the data
		}

		//image loaded from png
		imageT(std::string filepath) {
			std::vector<unsigned char> decodedData;

			bool hasAlpha = false;
			if (!loadPngImage(filepath.c_str(), _width, _height, hasAlpha, decodedData)) {
				throw std::runtime_error(("Image file \"" + filepath + "\" is not available or is empty").c_str());
			}
			uint32_t dN = hasAlpha ? 4 : 3;

			//process the decoded data
			_userData.resize(_width * _height);

			if (std::is_integral<T>::value) { //copy value
				for (uint32_t i = 0; i < _userData.size(); i++) {
					T* px = (T*) & (_userData[i]);
					for (int u = 0; u < N; u++) {
						if (u < dN) {
							px[u] = decodedData[i * dN + u];
						}
						else {
							px[u] = T(0);
						}
					}
				}
			}
			else if (std::is_floating_point<T>::value) { //normalize to range [0,1]
				for (uint32_t i = 0; i < _userData.size(); i++) {
					for (uint32_t u = 0; u < N; u++) {
						if (u < dN) {
							unsigned char decodedVal = decodedData[i * dN + u];
							((T*)& _userData[i])[u] = T(decodedVal) / T(255);
						}
						else {
							((T*)& _userData[i])[u] = T(0);
						}
					}
				}
			}
			else {
				for (int i = 0; i < (int)_userData.size(); i++) { //use constructor taking unsigned char
					for (uint32_t u = 0; u < N; u++) {
						if (u < dN) {
							unsigned char decodedVal = decodedData[i * dN + u];
							((T*)& _userData[i])[u] = T(decodedVal);
						}
						else {
							((T*)& _userData[i])[u] = T(0);
						}
					}
				}
			}
		}

		imageT(imageT && other) {
			_width = other._width;
			_height = other._height;
			_userData = std::move(other._userData);
		}
		imageT& operator=(imageT && other) {
			_width = other._width;
			_height = other._height;
			_userData = std::move(other._userData);
			return *this;
		}
	};

	using image1b = imageT<pixel1<unsigned char>>;
	using image2b = imageT<pixel2<unsigned char>>;
	using image3b = imageT<pixel3<unsigned char>>;
	using image4b = imageT<pixel4<unsigned char>>;

	using image1f = imageT<pixel1<float>>;
	using image2f = imageT<pixel2<float>>;
	using image3f = imageT<pixel3<float>>;
	using image4f = imageT<pixel4<float>>;

	using image1i = imageT<pixel1<int>>;
	using image2i = imageT<pixel2<int>>;
	using image3i = imageT<pixel3<int>>;
	using image4i = imageT<pixel4<int>>;

	using image1ui = imageT<pixel1<unsigned int>>;
	using image2ui = imageT<pixel2<unsigned int>>;
	using image3ui = imageT<pixel3<unsigned int>>;
	using image4ui = imageT<pixel4<unsigned int>>;

	using image = image3b; //default rgb 0-255 image
	using imagea = image4b; //default rgba 0-255 image
}