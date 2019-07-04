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
	struct aniBinding {
		const animation* ani;
		const nodeContainer* nodeCont;
		std::vector<int> nodes;
	};

	struct animationContainer {
		std::vector<animation> animations;
		std::map<string, int> nameMap;

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
		weight1Type =		(1 << 8)
	};

	class meshAttributeUtils {
	public:
		static uint32_t getAttribSize(meshAttributeUsage usage);
		static VkFormat getAttribFormat(meshAttributeUsage usage);
		static string	toString(meshAttributeUsage usage);
		static string	getAttribTypeName(meshAttributeUsage usage);
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
		vector<meshAttributeUsage> 					usages;			// usage of each attribute
		meshAttributeUsage 							allUsage;		// bitwise or of all usages
		vector<uint64_t> 							offsets;		// offsets in vertex memory
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
		vector<meshAttributeUsage> 					usages;			// usage of each attribute
		meshAttributeUsage 							allUsage;		// bitwise or of all usages
		vector<VkVertexInputAttributeDescription> 	attribs;		// attribute information
		vector<VkVertexInputBindingDescription> 	bindings;		// binding of each attriute

		// returns the index into the arrays of a certain usage
		// returns -1 if usage is not found
		int											findAttrib(meshAttributeUsage usage) const;

		// copy meshMemoryStructure but reorder its usages and offsets
		// to be the same usages, and eliminate any unneeded usages and offsets
		meshMemoryStructure							makePartialStructureView(meshMemoryStructure memStruct) const;

		// mapping is from usage type to shader location
		meshUsageLocationMapping(map<meshAttributeUsage, uint32_t> mapping);
	};

	class mesh {
	public:
		string					name;

	private:
		vector<uint16_t>		index;
		vector<vec3>			pos;
		vector<vec3>			norm;
		vector<vec3>			tangent;
		vector<vec2>			uv;
		vector<vec3>			color;
		vector<int32_t>			bone0;
		vector<int32_t>			bone1;
		vector<float>			weight0;
		vector<float>			weight1;

		nodeContainer			nodes;
		boneContainer			bones;
		vector<aniBinding>		animations;

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
		meshMemoryStructure			getDescription(uint64_t vertStart, uint64_t vertAlign, uint64_t indexStart, uint64_t indexAlign) const;
		float					getMaxRadius() const; // largest distance from origin

		// fill data
		void					fillVertexData(void* data, meshMemoryStructure desc) const;
		void					fillIndexData(void* data, meshMemoryStructure desc) const;

		void					calculateAnimationTransforms(int animationIndex, vector<mat4>& data, double time, behavior mode) const;
		bool					bindAnimation(const animation& ani);

		static void				convertAnimationFromCollada(string location, string outLocation);

		// create an empty mesh
		mesh();

		// load collada from file
		mesh(string path);
	};
}