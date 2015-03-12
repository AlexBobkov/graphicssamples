SET(LIB_NAME glimg)

SET(SRC_FILES
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

SET(HEADER_FILES
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

ADD_LIBRARY(${LIB_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
