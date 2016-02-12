set(SRC_FILES	
	${PROJECT_SOURCE_DIR}/common/Application.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Texture.cpp
)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/common/Application.hpp
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
	${PROJECT_SOURCE_DIR}/common/Texture.hpp
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