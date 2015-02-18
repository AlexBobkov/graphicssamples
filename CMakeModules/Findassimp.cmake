FIND_PATH(
	assimp_INCLUDE_DIRS
	NAMES assimp/scene.h
	PATHS /usr/local/include/
)

FIND_LIBRARY(
	assimp_LIBRARY
	NAMES assimp
	PATHS /usr/local/lib/
)

FIND_LIBRARY(
	assimp_LIBRARY_DEBUG
	NAMES assimpd
	PATHS /usr/local/lib/
)

IF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
    SET(assimp_FOUND TRUE)
ENDIF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)

IF (assimp_FOUND)
    IF (NOT assimp_FIND_QUIETLY)
        MESSAGE(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
    ENDIF (NOT assimp_FIND_QUIETLY)
ELSE (assimp_FOUND)
    IF (assimp_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find asset importer library")
    ENDIF (assimp_FIND_REQUIRED)
ENDIF (assimp_FOUND)

IF (assimp_LIBRARY_DEBUG)
	SET(assimp_LIBRARIES optimized ${assimp_LIBRARY} debug ${assimp_LIBRARY_DEBUG})
ELSE (assimp_LIBRARY_DEBUG)
	SET(assimp_LIBRARIES ${assimp_LIBRARY})
ENDIF (assimp_LIBRARY_DEBUG)