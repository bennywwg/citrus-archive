#pragma once

#include <memory>
#include <vector>
#include <map>

#include <assimp/scene.h>
#include "util.h"
#include "instance.h"

namespace citrus {
	struct node {
		std::string name;
		mat4 transform;
		int parent;
		std::vector<int> children;
	};

	struct nodeContainer {
		std::vector<node> nodes;
		std::map<std::string, int> nameMap;

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
		std::map<std::string, int> nameMap;
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
			std::string nodeName;
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

		std::string name;
		double begin, end;
		std::vector<channel> channels;

		void write(std::ostream& s) const;
		void read(std::istream& s);
	};

	//this references an animation
	//it has a list of node indices instead of
	//node names so it can be used to animate
	//nodes[i] corresponds to ani->channels[i]
	struct aniBinding {
		const animation* ani;
		const nodeContainer* nodeCont;
		std::vector<int> nodes;
	};

	struct animationContainer {
		std::vector<animation> animations;
		std::map<std::string, int> nameMap;

		void loadAllAnimations(const aiScene* scene);
	};

	enum class meshAttributeUsage : uint32_t {
		none =				0,
		positionType =		(1 << 0),
		normalType =		(1 << 1),
		tangentType =		(1 << 2),
		uvType =			(1 << 3),
		colorType =			(1 << 4),
		bone0Type =			(1 << 5),
		bone1Type =			(1 << 6),
		weight0Type =		(1 << 7),
		weight1Type =		(1 << 8),
		bitangentType =		(1 << 9)
	};

	class meshAttributeUtils {
	public:
		static uint32_t getAttribSize(meshAttributeUsage usage);
		static VkFormat getAttribFormat(meshAttributeUsage usage);
		static std::string	toString(meshAttributeUsage usage);
		static std::string	getAttribTypeName(meshAttributeUsage usage);
	};

	// these following structs warrant an explanation
	// the idea is the following steps are taken:
	// 1) load a mesh
	// 2) create a meshMemoryStructure
	// 3) use the meshMemoryStructure to load mesh onto GPU
	// 4) create meshUsageLocationMapping in accordance with shader attribs
	// 5) narrow meshMemoryStructure with meshUsageLocationMapping to only contain needed attribs
	// 6) draw

	// describes a mesh in terms of how its data is structured in memory
	// contains binding metadata for a mesh and associated utilities
	// there is no guarantee that offsets are ordered in any way
	struct meshMemoryStructure {
		std::vector<meshAttributeUsage> 					usages;			// usage of each attribute
		meshAttributeUsage 							allUsage;		// bitwise or of all usages
		std::vector<uint64_t> 							offsets;		// offsets in vertex memory
		uint64_t 									nextFree;		// next free vertex memory address
		uint64_t 									indexOffset;	// offset in index memory
		uint64_t 									nextFreeIndex;	// next free index memory address
		uint64_t 									vertCount;		// O_O
		uint64_t 									indexCount;		// O_O

		// returns the index into usages and offsets of a certain usage
		// returns -1 if usage is not found
		int											findAttrib(meshAttributeUsage usage) const;
	};

	struct meshUsageLocationMapping {
		std::vector<meshAttributeUsage> 					usages;			// usage of each attribute
		meshAttributeUsage 							allUsage;		// bitwise or of all usages
		std::vector<VkVertexInputAttributeDescription> 	attribs;		// attribute information
		std::vector<VkVertexInputBindingDescription> 	bindings;		// binding of each attriute

		// returns the index into the arrays of a certain usage
		// returns -1 if usage is not found
		int											findAttrib(meshAttributeUsage usage) const;

		// copy meshMemoryStructure but reorder its usages and offsets
		// to be the same usages, and eliminate any unneeded usages and offsets
		meshMemoryStructure							makePartialStructureView(meshMemoryStructure memStruct) const;

		meshUsageLocationMapping();
		// mapping is from usage type to shader location
		meshUsageLocationMapping(std::map<meshAttributeUsage, uint32_t> mapping);
	};

	class mesh {
	public:
		std::string					name;

		std::vector<uint16_t>		index;
		std::vector<vec3>			pos;
		std::vector<vec3>			norm;
		std::vector<vec3>			tangent;
		std::vector<vec3>			bitangent;
		std::vector<vec2>			uv;
		std::vector<vec3>			color;
		std::vector<int32_t>		bone0;
		std::vector<int32_t>		bone1;
		std::vector<float>			weight0;
		std::vector<float>			weight1;

	private:
		nodeContainer			nodes;
		boneContainer			bones;
		std::vector<aniBinding>	animations;

		bool					valid() const;

		void					loadNodes(const aiScene* scene, aiMesh* mesh);
		void					loadBones(const aiScene* scene, aiMesh* mesh);
		void					loadMesh(const aiScene* scene, aiMesh* mesh);

	public:
		bool					hasNorm() const;
		bool					hasTangent() const;
		bool					hasUV() const;
		bool					hasColor() const;
		bool					hasBones() const;
		
		void					fillEmpty();
		void					clearBones();

		// get description of mesh
		meshMemoryStructure		getDescription(uint64_t vertStart, uint64_t vertAlign, uint64_t indexStart, uint64_t indexAlign) const;
		float					getMaxRadius() const; // largest distance from origin

		// fill data
		void					fillVertexData(void* data, meshMemoryStructure desc) const;
		void					fillIndexData(void* data, meshMemoryStructure desc) const;

		void					calculateAnimationTransforms(int animationIndex, std::vector<mat4>& data, double time, behavior mode) const;
		bool					bindAnimation(const animation& ani);

		static void				convertAnimationFromCollada(std::string location, std::string outLocation);

		// create an empty mesh
		mesh();

		// load collada from file
		mesh(std::string path);
	};
}