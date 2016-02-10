cmake_minimum_required(VERSION 3.0)
 
project(GraphicsDependencies)
 
include(ExternalProject)

set_directory_properties(PROPERTIES EP_BASE ${CMAKE_BINARY_DIR}/External)

ExternalProject_Add(GLEW
	URL https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip/download
	CONFIGURE_COMMAND "${CMAKE_COMMAND}"
		"${CMAKE_BINARY_DIR}/External/Source/GLEW/build/cmake"
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")