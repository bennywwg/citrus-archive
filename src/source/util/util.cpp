#include <GLFW/glfw3.h>

#include <util/util.h>
#include <util/glmUtil.h>
#include <util/stdUtil.h>

#include <nfd.h>
#include <thread>

namespace citrus::util {
	bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, std::vector<unsigned char>& res) {
		res.clear();
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = 0;
		int color_type, interlace_type;
		FILE *fp;
	 
		if ((fp = fopen(name, "rb")) == NULL)
			return false;
	 
		/* Create and initialize the png_struct
		 * with the desired error handler
		 * functions.  If you want to use the
		 * default stderr and longjump method,
		 * you can supply NULL for the last
		 * three parameters.  We also supply the
		 * the compiler header file version, so
		 * that we know if the application
		 * was compiled with a compatible version
		 * of the library.  REQUIRED
		 */
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	 
		if (png_ptr == NULL) {
			fclose(fp);
			return false;
		}
	 
		/* Allocate/initialize the memory
		 * for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			fclose(fp);
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}
	 
		/* Set error handling if you are
		 * using the setjmp/longjmp method
		 * (this is the normal method of
		 * doing things with libpng).
		 * REQUIRED unless you  set up
		 * your own error handlers in
		 * the png_create_read_struct()
		 * earlier.
		 */
		if (setjmp(png_jmpbuf(png_ptr))) {
			/* Free all of the memory associated
			 * with the png_ptr and info_ptr */
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
			/* If we get here, we had a
			 * problem reading the file */
			return false;
		}
	 
		/* Set up the output control if
		 * you are using standard C streams */
		png_init_io(png_ptr, fp);
	 
		/* If we have already
		 * read some of the signature */
		png_set_sig_bytes(png_ptr, sig_read);
	 
		/*
		 * If you have enough memory to read
		 * in the entire image at once, and
		 * you need to specify only
		 * transforms that can be controlled
		 * with one of the PNG_TRANSFORM_*
		 * bits (this presently excludes
		 * dithering, filling, setting
		 * background, and doing gamma
		 * adjustment), then you can read the
		 * entire image (including pixels)
		 * into the info structure with this
		 * call
		 *
		 * PNG_TRANSFORM_STRIP_16 |
		 * PNG_TRANSFORM_PACKING  forces 8 bit
		 * PNG_TRANSFORM_EXPAND forces to
		 *  expand a palette into RGB
		 */
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	 
		png_uint_32 width, height;
		int bit_depth;
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
					 &interlace_type, NULL, NULL);
		outWidth = int(width);
		outHeight = int(height);
		outHasAlpha = color_type | PNG_COLOR_MASK_ALPHA;
	 
		unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		res.resize(outHeight * row_bytes);
		unsigned char* outData = res.data();
	 
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
		
				 
		for (int i = 0; i < outHeight; i++) {
			// note that png is ordered top to
			// bottom, but OpenGL expect it bottom to top
			// so the order or swapped
			memcpy(outData+(row_bytes * (outHeight-1-i)), row_pointers[i], row_bytes);
		}
		
	 
		/* Clean up after the read,
		 * and free any memory allocated */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	 
		/* Close the file */
		fclose(fp);
	 
		/* That's it */
		return true;
	}
	
	mutex _soutMut;
	mutex _idMut;
	int _currentID = 0;

	string selectFolder() {
		nfdchar_t *outPath = nullptr;
		nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

		if(result == NFD_OKAY) {
			string res = outPath;
			free(outPath);
			return res;
		} else if(result == NFD_CANCEL) {
			return "";
		} else {
			return "";
		}			
	}
	string selectFile(string filter) {
		nfdchar_t *outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog(filter.empty() ? nullptr : filter.c_str(), nullptr, &outPath);

		if(result == NFD_OKAY) {
			string res = outPath;
			free(outPath);
			return res;
		} else if(result == NFD_CANCEL) {
			return "";
		} else {
			return "";
		}
	}

	int nextID() {
		std::lock_guard<mutex> lock(_idMut);
		_currentID++;
		return _currentID;
	}

	vec3 btToGlm(btVector3 vec) {
		return vec3(vec.getX(), vec.getY(), vec.getZ());
	}
	btVector3 glmToBt(vec3 vec) {
		return btVector3(vec.x, vec.y, vec.z);
	}
	transform btToGlm(btTransform tr) {
		return transform(btToGlm(tr.getOrigin()), btToGlm(tr.getRotation()));
	}

	quat btToGlm(btQuaternion quat) {
		return glm::quat(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
	}
	btQuaternion glmToBt(quat quat) {
		return btQuaternion(quat.x, quat.y, quat.z, quat.w);
	}
	btTransform glmToBt(transform tr) {
		return btTransform(glmToBt(tr.getOrientation()), glmToBt(tr.getPosition()));
	}
	
	vector<btVector3> glmToBtVector(const vector<vec3>& verts) {
		vector<btVector3> btVerts;
		btVerts.resize(verts.size());
		for(size_t i = 0; i < verts.size(); i++) {
			btVerts[i] = btVector3(verts[i].x, verts[i].y, verts[i].z);
		}
		return btVerts;
	}

	bool isPowerOfTwo(unsigned int val) {
		return (val != 0) && ((val & (val - 1)) == 0);
	}

	string loadEntireFile(string path) {
		std::ifstream f(path, std::ios::binary);
		if(!f.is_open()) throw std::runtime_error("Couldn't open file: " + path);
		string res;	
		f.seekg(0, std::ios::end);
		res.reserve(size_t(f.tellg()));
		f.seekg(0, std::ios::beg);
		res.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
		return res;
	}

	void sout(string text) {
		std::lock_guard<mutex> lock(_soutMut);
		std::cout << text;
	}

	void spin_until(std::function<bool()> func, std::chrono::microseconds wait) {
		while(!func()) std::this_thread::sleep_for(wait);
	}

	std::string keyString(int key, int scancode, int action, int mods) {
		std::string res;
		if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
			res = std::string(1, 'A' + key - GLFW_KEY_A);
		} else if(key == GLFW_KEY_LEFT_SHIFT) {
			res = "LShift";
		} else if(key == GLFW_KEY_RIGHT_SHIFT) {
			res = "RShift";
		} else if(key == GLFW_KEY_TAB) {
			res = "Tab";
		} else if(key == GLFW_KEY_ENTER) {
			res = "Return";
		} else if(key == GLFW_KEY_RIGHT) {
			res = "(Right)";
		} else if(key == GLFW_KEY_UP) {
			res = "(Up)";
		} else if(key == GLFW_KEY_LEFT) {
			res = "(Left)";
		} else if(key == GLFW_KEY_DOWN) {
			res = "(Down)";
		} else if(key == GLFW_KEY_SPACE) {
			res = "Space";
		} else if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
			res = std::string(1, '0' + key - GLFW_KEY_0);
		} else if(key == GLFW_KEY_MINUS) {
			res = "-";
		} else if(key == GLFW_KEY_EQUAL) {
			res = "=";
		} else if(key == GLFW_KEY_LEFT_BRACKET) {
			res = "[";
		} else if(key == GLFW_KEY_RIGHT_BRACKET) {
			res = "]";
		} else if(key == GLFW_KEY_BACKSLASH) {
			res = "\\";
		} else if(key == GLFW_KEY_COMMA) {
			res = ",";
		} else if(key == GLFW_KEY_PERIOD) {
			res = ".";
		} else if(key == GLFW_KEY_SLASH) {
			res = "/";
		} else if(key == GLFW_KEY_GRAVE_ACCENT) {
			res = "~";
		} else if(key == GLFW_KEY_BACKSPACE) {
			res = "Back";
		} else if(key == GLFW_KEY_SEMICOLON) {
			res = ";";
		} else if(key == GLFW_KEY_APOSTROPHE) {
			res = "'";
		} else if(key == GLFW_KEY_ESCAPE) {
			res = "Esc";
		}
		return res;
	}

	scopedProfiler::scopedProfiler(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) { }
	scopedProfiler::~scopedProfiler() {
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		//util::sout(name + (name.size() < 32 ? std::string(32 - name.size(), ' ') : " ") + " took " + std::to_string(elapsed.count()) + "ns");
	}
}