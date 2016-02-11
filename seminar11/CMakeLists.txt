set(SRC_FILES	
	Application.cpp		
	Texture.cpp
	Framebuffer.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
)

set(HEADER_FILES
	Application.h		
	Texture.h
	Framebuffer.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
)

set(SHADER_FILES
	shaders11/oculus.vert	
	shaders11/oculus2.vert	
	shaders11/oculus.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_11_1_HorizontalSplit)

if (OCULUS_SDK_LIBRARIES)
	MAKE_SAMPLE(Sample_11_2_Oculus)
		
	target_link_libraries(Sample_11_2_Oculus ${OCULUS_SDK_LIBRARIES})
endif ()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar11/shaders11 DESTINATION ${CMAKE_INSTALL_PREFIX})