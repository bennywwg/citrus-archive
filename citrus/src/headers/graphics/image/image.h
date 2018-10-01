#pragma once

#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>

#include <glm\ext.hpp>

#include <lodepng/lodepng.h>

#include <util/typeEnums.h>
#include <util/util.h>

namespace citrus {

	namespace graphics {
		#pragma pack(push, 1)
		template<typename V>
		struct pixel1 {
			V r;
			const static GLint format = GL_RED;
		};
		template<typename V>
		struct pixel2 {
			V r, g;
			const static GLint format = GL_RG;
		};
		template<typename V>
		struct pixel3 {
			V r, g, b;
			const static GLint format = GL_RGB;
		};
		template<typename V>
		struct pixel4 {
			V r, g, b, a;
			const static GLint format = GL_RGBA;
		};
		#pragma pack(pop)

		template<typename P>
		class imageT {
			using T = decltype(((P*)nullptr)->r);

		private:
			unsigned int _width, _height;
			std::vector<P> _data;

			void exceptIfOutOfBounds(unsigned int x, unsigned int y) const {
				if (x >= _width || y >= _height) throw std::string("Image access out of bounds");
			}

		public:
			const P* data() const {
				return _data.data();
			}
			P* data() {
				return _data.data();
			}

			size_t size() const {
				return _data.size() * sizeof(P);
			}
			size_t pixelCount() const {
				return _data.size();
			}

			unsigned int width() const {
				return _width;
			}
			unsigned int height() const {
				return _height;
			}

			const P& at(unsigned int x, unsigned int y) const {
				exceptIfOutOfBounds(x, y);

				return _data[y * _width + x];
			}
			P& at(unsigned int x, unsigned int y) {
				exceptIfOutOfBounds(x, y);

				return _data[y * _width + x];
			}

			unsigned int getNumComponents() const {
				return sizeof(P) / sizeof(T);
			}

			GLint glFormat() const {
				return P::format;
			}
			GLint glType() const {
				return languageTypeTraits<T>::glType;
			}

			//black image
			imageT(unsigned int width, unsigned int height) : _width(width), _height(height) {
				if (width == 0 || height == 0) throw std::string("Width and Height of image must be nonzero");

				_data.resize(width * height);
				memset(_data.data(), 0, _data.size() * sizeof(P)); //zero the data
			}

			//image loaded from png
			imageT(std::string filepath) {
				std::vector<unsigned char> fileData;
				lodepng::load_file(fileData, filepath);
				if (fileData.size() == 0) throw std::exception(("Image file \"" + filepath + "\" is not available or is empty").c_str());

				std::vector<unsigned char> decodedData;
				if (lodepng::decode(decodedData, _width, _height, fileData)) throw std::exception(("PNG \"" + filepath + "\" couldn't be decoded").c_str()); 

				//process the decoded data
				_data.resize(_width * _height);

				if (decodedData.size() == (_width * _height * sizeof(P))) {
					memcpy(_data.data(), decodedData.data(), decodedData.size()); //if both are the same component size just copy
				} else {
					if (std::is_integral<T>::value) { //copy value
						for (int i = 0; i < _data.size(); i++) {
							for (int u = 0; u < sizeof(P) && u < 4; u++) {
								((T*)&_data[i])[u] = decodedData[i * 4 + u];
							}
						}
					} else if (std::is_floating_point<T>::value) { //normalize to range [0,1]
						for (int i = 0; i < _data.size(); i++) {
							for (int u = 0; u < sizeof(P) && u < 4; u++) {
								unsigned char decodedVal = decodedData[i * 4 + u];
								((T*)&_data[i])[u] = T(decodedVal) / T(255);
							}
						}
					} else {
						for (int i = 0; i < _data.size(); i++) { //use constructor taking unsigned char
							for (int u = 0; u < sizeof(P) && u < 4; u++) {
								unsigned char decodedVal = decodedData[i * 4 + u];
								((T*)&_data[i])[u] = T(decodedVal);
							}
						}
					}
				}
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
}

#endif