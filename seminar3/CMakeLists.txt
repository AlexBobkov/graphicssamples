SET(APP_NAME seminar3)

SET(SRC_FILES	
	Application.cpp
	Mesh.cpp
	ShaderProgram.cpp
)

SET(HEADER_FILES
	Application.h
	Mesh.h
	ShaderProgram.h
)

SET(SHADER_FILES
	shaders3/shader.vert
	shaders3/shader.frag	
	shaders3/shaderDiscard.frag
	shaders3/shaderLine.vert
	shaders3/shaderPoint.vert
	shaders3/shaderQuad.vert
	shaders3/shaderQuad.frag
	shaders3/shaderQuadMandelbrot.frag
	shaders3/shaderTime.vert
	shaders3/shaderTime.frag
	shaders3/shaderTimeCoord.vert
	shaders3/shaderTimeCoord.frag
	shaders3/simple.vert
	shaders3/simple.frag
	shaders3/simpleMat.vert
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_3_1)
MAKE_SAMPLE(Sample_3_2)
MAKE_SAMPLE(Sample_3_3)
MAKE_SAMPLE(Sample_3_4)

IF (USE_GLSL_130)
	INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders3_130/ DESTINATION ${CMAKE_INSTALL_PREFIX}/shaders3)
ELSE ()
	INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders3 DESTINATION ${CMAKE_INSTALL_PREFIX})
ENDIF ()