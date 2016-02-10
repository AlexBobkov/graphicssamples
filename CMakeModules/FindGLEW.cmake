ExternalProject_Add(GLEW
	PREFIX ${CMAKE_BINARY_DIR}/External
	URL https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip/download
	CONFIGURE_COMMAND "${CMAKE_COMMAND}"
		"${CMAKE_BINARY_DIR}/External/src/GLEW/build/cmake"
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/External")

add_library(glew STATIC IMPORTED)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/External/include")

set_target_properties(glew PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "GLEW_STATIC"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/External/include"
)

if (WIN32)	
	set_target_properties(glew PROPERTIES
		IMPORTED_LOCATION_RELEASE "${CMAKE_BINARY_DIR}/External/lib/libglew32.lib"
		IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "glu32;opengl32")
		
	set_target_properties(glew PROPERTIES
		IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/External/lib/libglew32d.lib"
		IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "glu32;opengl32")
else ()
endif ()
	
#find_library(GLEW_LIBRARY_RELEASE NAMES libglew32 PATHS ${CMAKE_BINARY_DIR}/External/lib)
#find_library(GLEW_LIBRARY_DEBUG NAMES libglew32d PATHS ${CMAKE_BINARY_DIR}/External/lib)	
#set_property(TARGET glew PROPERTY IMPORTED_LOCATION_RELEASE ${GLEW_LIBRARY_RELEASE})
#set_property(TARGET glew PROPERTY IMPORTED_LOCATION_DEBUG   ${GLEW_LIBRARY_DEBUG})
