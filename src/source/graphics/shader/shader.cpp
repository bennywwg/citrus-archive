#include <graphics/shader/shader.h>
#include <graphics/texture/texture.h>

namespace citrus {
	namespace graphics {
		GLuint vertexshader::ptr() {
			return _ptr;
		}
		bool vertexshader::good() const {
			return _good;
		}
		string vertexshader::log() const {
			return _log;
		}
		vertexshader::vertexshader() {
			_ptr = glCreateShader(GL_VERTEX_SHADER);
		}
		void vertexshader::compile(string src) {
			char const* srcptr = src.c_str();
			glShaderSource(_ptr, 1, &srcptr, NULL);
			glCompileShader(_ptr);

			GLint status = GL_FALSE;
			glGetShaderiv(_ptr, GL_COMPILE_STATUS, &status);
			_good = status == GL_TRUE;

			int logLength;
			glGetShaderiv(_ptr, GL_INFO_LOG_LENGTH, &logLength);
			if(logLength > 0) {
				vector<char> log(logLength + 1);
				glGetShaderInfoLog(_ptr, logLength, NULL, log.data());
				_log = string(log.data());
			}
		}
		vertexshader::vertexshader(standardVertexShader standardType) : vertexshader() {
			if(standardType == positionPassthrough) {
				compile(
					"#version 330 core\n"
					"layout(location = 0) in vec3 vert_pos;\n"
					"void main() {\n"
					"  gl_Position.xyz = vert_pos;\n"
					"  gl_Position.z = 1;\n"
					"}\n"
				);
			}
		}
		vertexshader::vertexshader(string src) : vertexshader() {
			compile(src);
		}
		vertexshader::~vertexshader() {
			glDeleteShader(_ptr);
		}

		GLuint geometryShader::ptr() {
			return _ptr;
		}
		bool geometryShader::good() const {
			return _good;
		}
		string geometryShader::log() const {
			return _log;
		}
		geometryShader::geometryShader(string src) {
			_ptr = glCreateShader(GL_GEOMETRY_SHADER);

			char const* srcptr = src.c_str();
			glShaderSource(_ptr, 1, &srcptr, NULL);
			glCompileShader(_ptr);

			GLint status = GL_FALSE;
			glGetShaderiv(_ptr, GL_COMPILE_STATUS, &status);
			_good = status == GL_TRUE;

			int logLength;
			glGetShaderiv(_ptr, GL_INFO_LOG_LENGTH, &logLength);
			if(logLength > 0) {
				vector<char> log(logLength + 1);
				glGetShaderInfoLog(_ptr, logLength, NULL, log.data());
				_log = string(log.data());
			}
		}
		geometryShader::~geometryShader() {
			glDeleteShader(_ptr);
		}

		GLuint fragmentshader::ptr() {
			return _ptr;
		}
		bool fragmentshader::good() const {
			return _good;
		}
		string fragmentshader::log() const {
			return _log;
		}
		fragmentshader::fragmentshader(string src) {
			_ptr = glCreateShader(GL_FRAGMENT_SHADER);

			char const* srcptr = src.c_str();
			glShaderSource(_ptr, 1, &srcptr, NULL);
			glCompileShader(_ptr);

			GLint status = GL_FALSE;
			glGetShaderiv(_ptr, GL_COMPILE_STATUS, &status);
			_good = status == GL_TRUE;

			int logLength;
			glGetShaderiv(_ptr, GL_INFO_LOG_LENGTH, &logLength);
			if(logLength > 0) {
				vector<char> log(logLength + 1);
				glGetShaderInfoLog(_ptr, logLength, NULL, log.data());
				_log = string(log.data());
			}
		}
		fragmentshader::~fragmentshader() {
			glDeleteShader(_ptr);
		}

		const shader* shader::_currentBound = nullptr;
		GLuint shader::_nextUnitAvailable = GL_TEXTURE0;
		GLuint shader::ptr() {
			return _ptr;
		}
		bool shader::good() const {
			return _good;
		}
		string shader::log() const {
			return _log;
		}
		void shader::use() const {
			if(_currentBound != this) {
				glUseProgram(_ptr);
				_currentBound = this;
				_nextUnitAvailable = GL_TEXTURE0;
			}
		}
		void shader::unuse() {
			glUseProgram(0);
			_currentBound = nullptr;
			_nextUnitAvailable = GL_TEXTURE0;
		}
		void shader::setUniform(string name, float f) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform1f(loc, f);
		}
		void shader::setUniform(string name, vec2 v) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform2f(loc, v.x, v.y);
		}
		void shader::setUniform(string name, vec3 v) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform3f(loc, v.x, v.y, v.z);
		}
		void shader::setUniform(string name, vec4 v) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform4f(loc, v.x, v.y, v.z, v.w);
		}
		void shader::setUniform(string name, int i) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform1i(loc, i);
		}
		void shader::setUniform(string name, ivec2 v) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform2i(loc, v.x, v.y);
		}
		void shader::setUniform(string name, ivec3 v) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform3i(loc, v.x, v.y, v.z);
		}
		void shader::setSampler(string name, const texture& tex) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			tex.bind(_nextUnitAvailable);
			glUniform1i(loc, _nextUnitAvailable - GL_TEXTURE0);
			_nextUnitAvailable++;
			if(_nextUnitAvailable > GL_TEXTURE31) {
				throw std::runtime_error("You can't use more than 32 textures in one shader pass");
			}
		}
		void shader::setUniform(string name, unsigned int i) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniform1ui(loc, i);
		}
		void shader::setUniform(string name, mat4 m) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
		}
		void shader::setUniform(string name, mat4* ms, int count) {
			use();
			GLuint loc = glGetUniformLocation(_ptr, name.c_str());
			glUniformMatrix4fv(loc, count, GL_FALSE, (GLfloat*)ms);
		}
		shader::shader() {
			_ptr = glCreateProgram();
		}
		void shader::link(vertexshader * vert, fragmentshader * frag) {
			_log = "";
			if(vert->good() && frag->good()) {
				glAttachShader(_ptr, vert->ptr());
				glAttachShader(_ptr, frag->ptr());
				glLinkProgram(_ptr);

				GLint res = GL_FALSE;
				glGetProgramiv(_ptr, GL_LINK_STATUS, &res);
				_good = res == GL_TRUE;

				int logLength;
				glGetProgramiv(_ptr, GL_INFO_LOG_LENGTH, &logLength);
				if(logLength > 0) {
					vector<char> log(logLength + 1);
					glGetProgramInfoLog(_ptr, logLength, NULL, log.data());
					_log = string(log.data());
				}

				glDetachShader(_ptr, vert->ptr());
				glDetachShader(_ptr, frag->ptr());
			} else {
				_good = false;
				_log = "Shader Error:\nIncorrectly compiled shaders given as arguments:\n";
				if(!(vert->good())) {
					_log += "Vertex Error:\n";
					_log += vert->log();
				}
				if(!(frag->good())) {
					if(_log[_log.size() - 1] != '\n') _log += "\n";
					_log += "Fragment Error:\n";
					_log += frag->log();
				}
				if(_log[_log.size() - 1] != '\n') _log += "\n";
			}
		}
		void shader::link(vertexshader * vert, geometryShader* geom, fragmentshader * frag) {
			_log = "";
			if(vert->good() && geom->good() && frag->good()) {
				glAttachShader(_ptr, vert->ptr());
				glAttachShader(_ptr, geom->ptr());
				glAttachShader(_ptr, frag->ptr());
				glLinkProgram(_ptr);

				GLint res = GL_FALSE;
				glGetProgramiv(_ptr, GL_LINK_STATUS, &res);
				_good = res == GL_TRUE;

				int logLength;
				glGetProgramiv(_ptr, GL_INFO_LOG_LENGTH, &logLength);
				if(logLength > 0) {
					vector<char> log(logLength + 1);
					glGetProgramInfoLog(_ptr, logLength, NULL, log.data());
					_log = string(log.data());
				}

				glDetachShader(_ptr, vert->ptr());
				glDetachShader(_ptr, geom->ptr());
				glDetachShader(_ptr, frag->ptr());
			} else {
				_good = false;
				_log = "Shader Error:\nIncorrectly compiled shaders given as arguments:\n";
				if(!(vert->good())) {
					_log += "Vertex Error:\n";
					_log += vert->log();
				}
				if(!(geom->good())) {
					if(_log[_log.size() - 1] != '\n') _log += "\n";
					_log += "Geometry Error:\n";
					_log += geom->log();
				}
				if(!(frag->good())) {
					if(_log[_log.size() - 1] != '\n') _log += "\n";
					_log += "Fragment Error:\n";
					_log += frag->log();
				}
				if(_log[_log.size() - 1] != '\n') _log += "\n";
			}
		}

		shader::shader(string vertSrc, string fragSrc) : shader() {
			vertexshader vert(vertSrc);
			fragmentshader frag(fragSrc);
			link(&vert, &frag);
		}
		shader::shader(string vertSrc, string geomSrc, string fragSrc) : shader() {
			vertexshader vert(vertSrc);
			geometryShader geom(geomSrc);
			fragmentshader frag(fragSrc);
			link(&vert, &geom, &frag);
		}
		shader::shader(vertexshader * vert, fragmentshader * frag) : shader() {
			link(vert, frag);
		}
		shader::~shader() {
			glDeleteProgram(_ptr);
			if(_currentBound == this) {
				_currentBound = nullptr;
				_nextUnitAvailable = GL_TEXTURE0;
			}
		}
	}
}