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
	shaders5/projector.vert	
	shaders5/projector.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_05_1_Viewports)
MAKE_SAMPLE(Sample_05_2_ZFighting)
MAKE_SAMPLE(Sample_05_3_BackFaceCulling)
MAKE_SAMPLE(Sample_05_4_Transparency)
MAKE_SAMPLE(Sample_05_5_Stencil)
MAKE_SAMPLE(Sample_05_6_CSG)
MAKE_SAMPLE(Sample_05_7_Projector)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar5/shaders5 DESTINATION ${CMAKE_INSTALL_PREFIX})