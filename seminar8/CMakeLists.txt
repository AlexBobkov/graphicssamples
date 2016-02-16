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
	shaders8/toshadow.vert	
	shaders8/toshadow.frag
	
	shaders8/shadow.vert
	shaders8/shadow.frag
	shaders8/shadow2.frag
	
	shaders8/togbuffer.vert	
	shaders8/togbuffer.frag
	
	shaders8/deferred.vert
	shaders8/deferred.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_08_1_Shadows)
MAKE_SAMPLE(Sample_08_2_Deferred)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar8/shaders8 DESTINATION ${CMAKE_INSTALL_PREFIX})