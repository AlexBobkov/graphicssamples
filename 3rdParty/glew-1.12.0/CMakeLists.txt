SET(LIB_NAME glew)

SET(SRC_FILES
	src/glew.c	
)

SET(HEADER_FILES
	include/GL/glew.h
	include/GL/glxew.h
	include/GL/wglew.h
)

ADD_LIBRARY(${LIB_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
