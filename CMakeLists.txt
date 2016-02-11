cmake_minimum_required(VERSION 3.0)

project(GraphicsSamples VERSION 3.0)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

include(MacroUtils)

set(DEPENDENCIES_ROOT CACHE PATH "Dependencies install path")
if (DEPENDENCIES_ROOT)
	include(${DEPENDENCIES_ROOT}/lib/cmake/glew/glew-targets.cmake)
	include(${DEPENDENCIES_ROOT}/lib/cmake/glfw/glfwTargets.cmake)
	include(${DEPENDENCIES_ROOT}/lib/cmake/glm/glmTargets.cmake)
	include(${DEPENDENCIES_ROOT}/lib/cmake/assimp-3.2/assimpTargets.cmake)
else ()
	message("Set DEPENDENCIES_ROOT variable")
endif ()
		
find_package(AntTweakBar)
find_package(OculusSDK)

if (UNIX)
	find_package(X11 REQUIRED)

	find_package(OpenGL REQUIRED)
	add_definitions(${OpenGL_DEFINITIONS})

	find_package(Threads REQUIRED)
endif (UNIX)

set(CMAKE_DEBUG_POSTFIX "d")

option(USE_CORE_PROFILE "Enable to use core profile" OFF)
if (USE_CORE_PROFILE)
	add_definitions(-DUSE_CORE_PROFILE)
endif (USE_CORE_PROFILE)

option(USE_GLSL_130 "Enable to use GLSL version 130" OFF)

include_directories(
	${OpenGL_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/3rdParty/glimg-0.5.2/include
	${ANTTWEAKBAR_INCLUDE_DIRS}
)

if (OCULUS_SDK_LIBRARIES)
	include_directories(${INCLUDE_DIRECTORIES} ${OCULUS_SDK_INCLUDE_DIRS})
endif ()

link_directories(
	${OpenGL_LIBRARY_DIRS}
)

add_subdirectory(3rdParty/glimg-0.5.2)
add_subdirectory(seminar1)
add_subdirectory(seminar2)
add_subdirectory(seminar3)
add_subdirectory(seminar4)
add_subdirectory(seminar5)
add_subdirectory(seminar6)
add_subdirectory(seminar7)
add_subdirectory(seminar8)
add_subdirectory(seminar9)
add_subdirectory(seminar10)
add_subdirectory(seminar11)

file(GLOB STUDENTS students/*)
foreach(STUDENT ${STUDENTS})
	if (IS_DIRECTORY ${STUDENT})
		message("Student task found " ${STUDENT})
		add_subdirectory("${STUDENT}")
	endif ()
endforeach()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/images/ DESTINATION ${CMAKE_INSTALL_PREFIX}/images)

# Здесь нужные библиотеки (dll) копируются из папки с зависимостями в папку install для примеров
if (WIN32)
	get_property(GLFW_LOCATION_DEBUG TARGET glfw PROPERTY IMPORTED_LOCATION_DEBUG)
	if (GLFW_LOCATION_DEBUG)
		install(FILES ${GLFW_LOCATION_DEBUG} DESTINATION ${CMAKE_INSTALL_PREFIX})
	else ()
		message("GLFW_LOCATION_DEBUG is empty")
	endif ()

	get_property(GLFW_LOCATION_RELEASE TARGET glfw PROPERTY IMPORTED_LOCATION_RELEASE)
	if (GLFW_LOCATION_RELEASE)
		install(FILES ${GLFW_LOCATION_RELEASE} DESTINATION ${CMAKE_INSTALL_PREFIX})
	else ()
		message("GLFW_LOCATION_RELEASE is empty")
	endif ()

	get_property(ASSIMP_LOCATION_DEBUG TARGET ASSIMP::assimp PROPERTY IMPORTED_LOCATION_DEBUG)
	if (ASSIMP_LOCATION_DEBUG)
		install(FILES ${ASSIMP_LOCATION_DEBUG} DESTINATION ${CMAKE_INSTALL_PREFIX})
	else ()
		message("ASSIMP_LOCATION_DEBUG is empty")
	endif ()

	get_property(ASSIMP_LOCATION_RELEASE TARGET ASSIMP::assimp PROPERTY IMPORTED_LOCATION_RELEASE)
	if (ASSIMP_LOCATION_RELEASE)
		install(FILES ${ASSIMP_LOCATION_RELEASE} DESTINATION ${CMAKE_INSTALL_PREFIX})
	else ()
		message("ASSIMP_LOCATION_RELEASE is empty")
	endif ()
endif (WIN32)

if (UNIX)
	set(CMAKE_INSTALL_RPATH "${DEPENDENCIES_ROOT}/lib")
endif ()