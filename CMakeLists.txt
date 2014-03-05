CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(DemoApplication)

FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
	PATH_SUFFIXES
		include
	PATHS
		/usr
		/usr/local
)

FIND_LIBRARY(GLEW_LIBRARY GLEW
	PATH_SUFFIXES
		lib
		lib64
	PATHS
		/usr
		/usr/local
)

FIND_PATH(GLFW_INCLUDE_DIR GLFW/glfw3.h
	PATH_SUFFIXES
		include
	PATHS
		/usr
		/usr/local
)

FIND_LIBRARY(GLFW_LIBRARY glfw
	PATH_SUFFIXES
		lib
		lib64
	PATHS
		/usr
		/usr/local
)

FIND_PATH(GLIMAGE_INCLUDE_DIR glimg/glimg.h
	PATH_SUFFIXES
		include
	PATHS
		/usr
		/usr/local
)

FIND_LIBRARY(GLIMAGE_LIBRARY glimg.lib
	PATH_SUFFIXES
		lib
		lib64
	PATHS
		/usr
		/usr/local
)

IF(NOT WIN32)
	FIND_LIBRARY(GL_LIBRARY GL
		PATH_SUFFIXES
			lib
			lib64
		PATHS
			/usr
			/usr/local
	)
ENDIF(NOT WIN32)

FIND_PATH(GLM_INCLUDE_DIR glm/glm.hpp
	PATH_SUFFIXES
		include
	PATHS
		/usr
		/usr/local
)

SET(CMAKE_INCLUDE_PATH CACHE STRING "Put your include dir here...")
SET(CMAKE_LIBRARY_PATH CACHE STRING "Put your lib dir here...")

SET(CMAKE_DEBUG_POSTFIX "d")

INCLUDE_DIRECTORIES(
	${CMAKE_INCLUDE_PATH}
	${GLEW_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}	
	${GLIMAGE_INCLUDE_DIR}
)

LINK_DIRECTORIES(${CMAKE_LIBRARY_PATH})

ADD_SUBDIRECTORY(seminar1)
ADD_SUBDIRECTORY(seminar2)
ADD_SUBDIRECTORY(seminar3)
ADD_SUBDIRECTORY(seminar4)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_INSTALL_PREFIX})