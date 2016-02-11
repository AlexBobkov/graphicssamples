cmake_minimum_required(VERSION 3.0)
 
project(GraphicsDependencies)

include(ExternalProject)

set_directory_properties(PROPERTIES EP_BASE ${CMAKE_BINARY_DIR}/External)

ExternalProject_Add(GLEW
	URL https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip/download
	CONFIGURE_COMMAND "${CMAKE_COMMAND}"
		"${CMAKE_BINARY_DIR}/External/Source/GLEW/build/cmake"
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
		"-DBUILD_UTILS:BOOL=OFF"
)

ExternalProject_Add(GLFW
	URL https://github.com/glfw/glfw/releases/download/3.1.2/glfw-3.1.2.zip
	#GIT_REPOSITORY https://github.com/glfw/glfw.git
	#GIT_TAG        3.1.2
	CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
		"-DCMAKE_DEBUG_POSTFIX:STRING=d"
		"-DBUILD_SHARED_LIBS:BOOL=ON"
)
		
ExternalProject_Add(GLM
	URL https://github.com/g-truc/glm/archive/0.9.7.2.zip
	#GIT_REPOSITORY https://github.com/g-truc/glm.git
	#GIT_TAG        0.9.7.2
	CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
)

ExternalProject_Add(Assimp
	URL https://github.com/AlexBobkov/assimp/archive/cmake.zip
	#GIT_REPOSITORY https://github.com/AlexBobkov/assimp.git
	#GIT_TAG        cmake
	CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
		"-DCMAKE_DEBUG_POSTFIX:STRING=d"
)

    