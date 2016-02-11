set(SRC_FILES	
	Application.cpp
	ShaderProgram.cpp
	Texture.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
)

set(HEADER_FILES
	Application.h
	ShaderProgram.h
	Texture.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
)

set(SHADER_FILES
	shaders7/projector.vert	
	shaders7/projector.frag
	
	shaders7/mrt.vert	
	shaders7/mrt.frag

	shaders7/quadDepth.vert	
	shaders7/quadDepth.frag

	shaders7/quadColor.vert
	shaders7/quadColor.frag	
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_7_1_Projector)
MAKE_SAMPLE(Sample_7_2_CopyTex)
MAKE_SAMPLE(Sample_7_3_Framebuffer)
MAKE_SAMPLE(Sample_7_4_MRT)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar7/shaders7 DESTINATION ${CMAKE_INSTALL_PREFIX})