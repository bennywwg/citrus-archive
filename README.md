# citrus
An ECS game engine using a custom multithreaded vulkan render engine.

# Status
With great regret, I have removed official support for linux; Only Windows 10 is officially supported for the time being. The linux version was clunky and had many problems, and was harder to set up than the windows version. The good news is the linux build should be as simple as collecting the required libraries (listed at the bottom of this readme) and running `g++ *.cpp` in a few places.

### 12/03/2019
* Refactored citrus into multiple Visual Studio Projects
* The projects, in order from least dependent to most dependent, are:
  * mankern - manager kernel, manages entities and element runtime
  * graphkern - graphics kernel, manages rendering runtime including all resources
  * builtin - collection of default entity and element functionality such as rendering and physics
  * editor - user interface runtime for modifying the game scene
  * game - the actual engine linking all afformentioned projects together
* Rewrote manager, entity, element, entRef, eleRef classes
* Adding and removing elements from entities after entity creation is now supported
* Basical serialization / deserialization tested
* Physics currently broken but should be easily fixed
* Editor shows scene hierarchy with basic hierarchy modification functions

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
