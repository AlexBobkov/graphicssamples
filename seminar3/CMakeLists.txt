set(SRC_FILES	
	${PROJECT_SOURCE_DIR}/common/Application.cpp
	${PROJECT_SOURCE_DIR}/common/Camera.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/common/Application.hpp
	${PROJECT_SOURCE_DIR}/common/Camera.hpp
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
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

MAKE_SAMPLE(Sample_03_1_Shaders)
MAKE_SAMPLE(Sample_03_2_Points)
MAKE_SAMPLE(Sample_03_3_Lines)
MAKE_SAMPLE(Sample_03_4_ScreenSpaceQuad)
MAKE_SAMPLE(Sample_03_5_UBO)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar3/shaders3 DESTINATION ${CMAKE_INSTALL_PREFIX})
