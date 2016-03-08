cmake_minimum_required(VERSION 3.0)
 
project(GraphicsDependencies)

set(CMAKE_DEBUG_POSTFIX "d")
set_directory_properties(PROPERTIES EP_BASE ${CMAKE_BINARY_DIR}/External)

find_package(Git)

include(ExternalProject)

#На некоторых дистрибутивах Линукса CMake собран без поддержки SSL и не может скачивать https.
#Поэтому приходится извращаться.

if (UNIX)

	ExternalProject_Add(GLEW
		DOWNLOAD_COMMAND wget https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip
		URL ${CMAKE_BINARY_DIR}/External/Download/GLEW/glew-1.13.0.zip
		CONFIGURE_COMMAND "${CMAKE_COMMAND}"
			"${CMAKE_BINARY_DIR}/External/Source/glew-1.13.0/build/cmake"
			"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
			"-DBUILD_UTILS:BOOL=OFF"
	)
	
	ExternalProject_Add_Step(GLEW extract
		DEPENDEES download
		COMMAND ${CMAKE_COMMAND} -E tar xfz ${CMAKE_BINARY_DIR}/External/Download/GLEW/glew-1.13.0.zip
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/External/Source
	)
		
	ExternalProject_Add(GLFW		
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG        3.1.2
		CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
			"-DCMAKE_DEBUG_POSTFIX:STRING=d"
			"-DBUILD_SHARED_LIBS:BOOL=ON"
	)
	
	ExternalProject_Add(GLM		
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG        0.9.7.2
		CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
			"-DCMAKE_INSTALL_LIBDIR:STRING=lib"
	)
	
	ExternalProject_Add(Assimp
		GIT_REPOSITORY https://github.com/AlexBobkov/assimp.git
		GIT_TAG        cmake
		CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
			"-DCMAKE_DEBUG_POSTFIX:STRING=d"
			"-DASSIMP_ENABLE_BOOST_WORKAROUND:BOOL=ON"
			"-DCMAKE_C_FLAGS=-fPIC"
			"-DCMAKE_CXX_FLAGS=-fPIC"
	)
	
	ExternalProject_Add(SOIL
		HG_REPOSITORY https://bitbucket.org/AlexBobkov/soil2
		HG_TAG        default
		CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
	)
	
else ()

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
		CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
			"-DCMAKE_DEBUG_POSTFIX:STRING=d"
			"-DBUILD_SHARED_LIBS:BOOL=ON"
	)
		
	ExternalProject_Add(GLM
		URL https://github.com/g-truc/glm/archive/0.9.7.2.zip	
		CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
	)

	ExternalProject_Add(Assimp
		URL https://github.com/AlexBobkov/assimp/archive/cmake.zip
		CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
			"-DCMAKE_DEBUG_POSTFIX:STRING=d"
			"-DASSIMP_BUILD_ASSIMP_TOOLS:BOOL=OFF"
	)

	ExternalProject_Add(SOIL
		URL https://bitbucket.org/AlexBobkov/soil2/get/default.zip
		CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
	)

endif ()