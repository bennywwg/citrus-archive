#pragma once

#include <memory>
#include <vector>

#include <assimp/scene.h>

#include "citrus/util.h"
#include "citrus/graphics/instance.h"

namespace citrus::graphics {
	struct node {
		std::string name;
		mat4 transform;
		int parent;
		std::vector<int> children;
	};

	struct nodeContainer {
		std::vector<node> nodes;
		std::map<string, int> nameMap;

	private:
		int addNodes(const aiNode* n, int parent = -1);

	public:
		void loadAllNodes(const aiScene* scene);
	};

	struct bone {
		int node;
		glm::mat4 offset;
	};

	struct vertexBoneInfo {
		int primary = -1, secondary = -1;
		float primaryWeight = 0.0f, secondaryWeight = 0.0f;
		void normalize();
	};

	struct boneContainer {
		std::vector<bone> bones;
		std::map<string, int> nameMap;
		std::vector<std::vector<float>> weights;

		vertexBoneInfo getInfo(unsigned int vertex);

		void loadAllBones(const aiScene* scene, const nodeContainer& nodes);
	};

	enum class behavior {
		base = 0,
		nearest = 1,
		repeat = 3,
		linear = 2
	};

	struct animation {
		template<typename T>
		struct key {
			double time;
			T val;
			static T mix(const double& time, const key<T>& l, const key<T>& r);
			key(const double& time, const T& val);
		};

		struct channel {
			string nodeName;
			double begin, end;
			std::vector<key<glm::vec3>> positions;
			std::vector<key<glm::quat>> orientations;
			std::vector<key<glm::vec3>> scalings;

			template<typename T>
			T get(double time, std::vector<key<T>>const& keys) const;

			vec3 getPosition(double time) const;
			quat getOrientation(double time) const;
			vec3 getScaling(double time) const;

			void write(std::ostream& s) const;
			void read(std::istream& s);
		};

		string name;
		double begin, end;
		std::vector<channel> channels;

		void write(std::ostream& s) const;
		void read(std::istream& s);
	};

	//this references an animation
	//it has a list of node indices instead of
	//node names so it can be used to animate
	//nodes[i] corresponds to ani->channels[i]
	struct animationBinding {
		const animation* ani;
		const nodeContainer* nodeCont;
		std::vector<int> nodes;
	};

	struct animationContainer {
		std::vector<animation> animations;
		std::map<string, int> nameMap;

		void loadAllAnimations(const aiScene* scene);
	};

	struct meshDescription {
		vector<VkVertexInputAttributeDescription> attribs;
		vector<VkVertexInputBindingDescription> bindings;

		string toString() const;

		static meshDescription getLit(bool rigged);
		static meshDescription getShadeless(bool rigged);
	};

	class mesh {
	public:
		string name;

		vector<uint16_t> index;
		vector<vec3> pos;
		vector<vec3> norm;
		vector<vec3> tangent;
		vector<vec2> uv;
		vector<int32_t> bone0;
		vector<int32_t> bone1;
		vector<float> weight0;
		vector<float> weight1;

		struct rawVertexData {
			void* dataStart;
			uint64_t dataSize;
			uint64_t elementCount;
		};

		vector<rawVertexData> getRawData();

		nodeContainer nodes;
		boneContainer bones;
		std::vector<animationBinding> animations;

		bool valid() const;

		bool hasNorm() const;
		bool hasTangent() const;
		bool hasUV() const;
		bool hasBones() const;

		void fillEmpty();
		void clearBones();

		uint64_t vertSizeWithoutPadding() const;
		uint64_t requiredMemory() const;
		void constructContinuous(void* data) const;
		void constructInterleaved(void* data) const;
		vector<uint64_t> offsets() const;

		//used for constructing constructing shaders and such
		meshDescription getDescription() const;

		//used to check if meshes and shaders share the same attributes
		uint64_t getDescriptionCode() const;

		void calculateAnimationTransforms(int animationIndex, std::vector<glm::mat4>& data, double time, behavior mode) const;
		bool bindAnimation(const animation& ani);

		void loadMesh(const aiScene* scene, aiMesh* mesh);
		void loadNodes(const aiScene* scene, aiMesh* mesh);
		void loadBones(const aiScene* scene, aiMesh* mesh);

		static void convertAnimationFromCollada(std::string location, std::string outLocation);

		mesh();
		mesh(std::string path);
	};
}