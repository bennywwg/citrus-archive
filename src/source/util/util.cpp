#include <GLFW/glfw3.h>

#include <util/util.h>
#include <util/glmUtil.h>
#include <util/stdUtil.h>

#include <nativefiledialog-master/src/include/nfd.h>

namespace citrus {
	namespace util {
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
		string selectFile() {
			nfdchar_t *outPath = nullptr;
			nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

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
			std::ifstream f(path);
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
}