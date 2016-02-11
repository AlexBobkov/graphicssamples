set(SRC_FILES	
	Application.cpp
	ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
)

set(HEADER_FILES
	Application.h
	ShaderProgram.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
)

set(SHADER_FILES
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

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_3_1_Shaders)
MAKE_SAMPLE(Sample_3_2_Points)
MAKE_SAMPLE(Sample_3_3_Lines)
MAKE_SAMPLE(Sample_3_4_ScreenSpaceQuad)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar3/shaders3 DESTINATION ${CMAKE_INSTALL_PREFIX})
