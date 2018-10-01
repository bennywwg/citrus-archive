#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>

#include <util/glmUtil.h>

#include <util/graphics_h.h>
#include <util/stdUtil.h>


namespace citrus {
	namespace graphics {
		enum standardVertexShader {
			positionPassthrough
		};

		class texture;

		class vertexshader {
			private:
			GLuint _ptr;
			bool _good;
			string _log;

			public:
			inline GLuint ptr();
			inline bool good() const;
			inline string log() const;

			private:
			vertexshader();
			void compile(string src);
			public:
			vertexshader(standardVertexShader standardType);
			vertexshader(string src);
			~vertexshader();

			NO_COPY(vertexshader)
		};

		class geometryShader {
			private:
			GLuint _ptr;
			bool _good;
			string _log;

			public:
			inline GLuint ptr();
			inline bool good() const;
			inline string log() const;

			geometryShader(string src);
			~geometryShader();

			NO_COPY(geometryShader)
		};

		class fragmentshader {
			private:
			GLuint _ptr;
			bool _good;
			string _log;
			public:
			GLuint ptr();
			bool good() const;
			string log() const;

			fragmentshader(string src);
			~fragmentshader();

			NO_COPY(fragmentshader)
		};

		class shader {
			private:
			//I can't think of any reason you'd want to NOT do this
			//but feel free to use setUniform(unsigned int) if you don't want to
			static const shader* _currentBound;
			static GLuint _nextUnitAvailable;

			private:
			GLuint _ptr;
			bool _good;
			string _log;

			public:
			GLuint ptr();
			bool good() const;
			string log() const;

			void use() const;
			static void unuse();

			void setUniform(string name, float f);
			void setUniform(string name, vec2 v);
			void setUniform(string name, vec3 v);
			void setUniform(string name, int i);
			void setUniform(string name, unsigned int i);
			//keeps track of allocating texture units, incrementing once after each texture bind
			void setSampler(string name, const texture& tex);
			void setUniform(string name, mat4 m);

			private:
			shader();
			void link(vertexshader* vert, fragmentshader* frag);
			void link(vertexshader* vert, geometryShader* geom, fragmentshader* frag);

			public:
			shader(string vertSrc, string fragSrc);
			shader(string vertSrc, string geomSrc, string fragSrc);
			shader(vertexshader* vert, fragmentshader* frag);
			~shader();

			NO_COPY(shader)
		};
	}
}

#endif