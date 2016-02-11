set(SRC_FILES
	source/DdsLoader.cpp
	source/DdsLoaderInt.h
	source/ImageCreator.cpp
	source/ImageFormat.cpp
	source/ImageSet.cpp
	source/ImageSetImpl.cpp
	source/ImageSetImpl.h
	source/OldDdsFmtConv.inc
	source/stb_image.c
	source/stb_image.h
	source/StbLoader.cpp
	source/TestLoader.cpp	
	source/Util.cpp
	source/Util.h
)

set(HEADER_FILES
	include/glimg/DdsLoader.h
	include/glimg/glimg.h
	include/glimg/ImageCreator.h
	include/glimg/ImageCreatorExceptions
	include/glimg/ImageFormat.h
	include/glimg/ImageSet.h
	include/glimg/Loaders.h
	include/glimg/StbLoader.h
	include/glimg/TestLoader.h
)

add_library(glimg STATIC ${SRC_FILES} ${HEADER_FILES})

if (CMAKE_COMPILER_IS_GNUCC)
	target_compile_options(glimg PUBLIC -std=c++11)
endif ()
