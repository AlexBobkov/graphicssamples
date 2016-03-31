set(SRC_FILES	
	${PROJECT_SOURCE_DIR}/common/Application.cpp
	${PROJECT_SOURCE_DIR}/common/Camera.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Texture.cpp
)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/common/Application.hpp
	${PROJECT_SOURCE_DIR}/common/Camera.hpp
	${PROJECT_SOURCE_DIR}/common/LightInfo.hpp
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
	${PROJECT_SOURCE_DIR}/common/Texture.hpp
)

set(SHADER_FILES
	shaders6/skybox.vert
	shaders6/skybox.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_06_1_CubeTex)
MAKE_SAMPLE(Sample_06_2_Viewports)
MAKE_SAMPLE(Sample_06_3_ZFighting)
MAKE_SAMPLE(Sample_06_4_BackFaceCulling)
MAKE_SAMPLE(Sample_06_5_Transparency)
MAKE_SAMPLE(Sample_06_6_Stencil)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar6/shaders6 DESTINATION ${CMAKE_INSTALL_PREFIX})