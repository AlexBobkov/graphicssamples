SET(APP_NAME seminar1)

SET(SRC_FILES	
	Application.cpp
)

SET(HEADER_FILES
	Application.h
)

SET(SHADER_FILES
	shaders1/simple.vert
	shaders1/simple.frag
	shaders1/cube.vert
	shaders1/triangle.vert
	shaders1/color.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_1_1_SolidTriangle)
MAKE_SAMPLE(Sample_1_2_ColoredTriangle)
MAKE_SAMPLE(Sample_1_3_DrawArrays)
MAKE_SAMPLE(Sample_1_4_DrawElements)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders1 DESTINATION ${CMAKE_INSTALL_PREFIX})
