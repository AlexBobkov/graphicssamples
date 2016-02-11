set(SRC_FILES	
	Application.cpp
	Texture.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
)

set(HEADER_FILES
	Application.h
	Texture.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
)

set(SHADER_FILES
	shaders6/common.vert	
	shaders6/common.frag
	
	shaders6/skybox.vert
	shaders6/skybox.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_6_1_CubeTex)
MAKE_SAMPLE(Sample_6_2_Viewports)
MAKE_SAMPLE(Sample_6_3_ZFighting)
MAKE_SAMPLE(Sample_6_4_BackFaceCulling)
MAKE_SAMPLE(Sample_6_5_Transparency)
MAKE_SAMPLE(Sample_6_6_Stencil)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar6/shaders6 DESTINATION ${CMAKE_INSTALL_PREFIX})