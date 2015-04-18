SET(APP_NAME seminar11)

SET(SRC_FILES	
	Application.cpp
	Mesh.cpp
	ShaderProgram.cpp
	Texture.cpp
	Framebuffer.cpp
)

SET(HEADER_FILES
	Application.h
	Mesh.h
	ShaderProgram.h
	Texture.h
	Framebuffer.h
)

SET(SHADER_FILES
	#shaders10/instancingNoMatrix.vert	
	#shaders10/instancingUniform.vert
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_11_1_HorizontalSplit)

IF (OCULUS_SDK_LIBRARIES)
	MAKE_SAMPLE(Sample_11_2_Oculus)
		
	TARGET_LINK_LIBRARIES(Sample_11_2_Oculus ${OCULUS_SDK_LIBRARIES})
ENDIF ()

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders11 DESTINATION ${CMAKE_INSTALL_PREFIX})