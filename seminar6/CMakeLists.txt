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
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
	${PROJECT_SOURCE_DIR}/common/Texture.hpp
)

set(SHADER_FILES
	shaders6/mrt.vert	
	shaders6/mrt.frag
    
    shaders6/toshadow.vert	
	shaders6/toshadow.frag
	
	shaders6/shadow.vert
	shaders6/shadow.frag
	shaders6/shadow2.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_06_1_CopyTex)
MAKE_SAMPLE(Sample_06_2_Framebuffer)
MAKE_SAMPLE(Sample_06_3_MRT)
MAKE_SAMPLE(Sample_06_4_Shadows)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar6/shaders6 DESTINATION ${CMAKE_INSTALL_PREFIX})