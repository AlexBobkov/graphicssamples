SET(APP_NAME seminar2)

SET(SRC_FILES	
	Application.cpp
)

SET(HEADER_FILES
	Application.h
)

SET(SHADER_FILES
	shaders2/shader.vert
	shaders2/shader.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_2_1)
#MAKE_SAMPLE(Sample_1_2)
#MAKE_SAMPLE(Sample_1_3)
#MAKE_SAMPLE(Sample_1_4)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders2 DESTINATION ${CMAKE_INSTALL_PREFIX})