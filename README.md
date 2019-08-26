# citrus
A multithreaded ECS game engine using a custom vulkan render engine.

Current status:
The port to vulkan is nearly complete. The critical features I pan on working on next are:
* Basic editor functionality such as scene graph and entity transformation
* Finish the basic lighting model (including point lights)
* Add shadows (point light shadows not planned)
* Scene / prefab saving loading

For other goals see the roadmap:

![roadmap](https://raw.githubusercontent.com/bennywwg/citrus/master/media/roadmap.png "Roadmap")

![sample](https://raw.githubusercontent.com/bennywwg/citrus/master/media/sample.png "Sample Scene")

![rigging](https://raw.githubusercontent.com/bennywwg/citrus/master/media/rigging.gif "Rigging")

Credit to the following libraries (and all of their dependencies)used in citrus:
* Bullet3D physics engine
* NativeFileDialog file picker
* libpng png loader
* glfw3 window and input abstraction layer
* glm matrix and vector math library for graphics
* assimp collada importer
* nlohmann json implementation for C++

Credit to toolkits used:
* GNU/Linux
* Visual Studio 2017 Community for code editing and compiling
* Blender for model and texture editing
* Paint.NET for image manipulation
