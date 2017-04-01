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
	shaders7/togbuffer.vert	
	shaders7/togbuffer.frag
	
	shaders7/deferred.vert
	shaders7/deferred.frag
    
    shaders7/deferredSphere.vert
	shaders7/deferredSphere.frag
    shaders7/deferredSphereDebug.frag
    
    shaders7/forwardLighting.vert
	shaders7/forwardLighting.frag
    
    shaders7/forwardLightingPrepass.vert
	shaders7/forwardLightingPrepass.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_07_1_Deferred)
MAKE_SAMPLE(Sample_07_2_Forward)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar7/shaders7 DESTINATION ${CMAKE_INSTALL_PREFIX})