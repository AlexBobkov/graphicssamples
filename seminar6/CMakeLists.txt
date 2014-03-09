SET(APP_NAME seminar6)

SET(SRC_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Application.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/CommonShader.cpp	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Meshes.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Program.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Shader.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/SkyBoxShader.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Textures.cpp
)

SET(HEADER_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Application.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/CommonShader.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Shader.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/SkyBoxShader.h
)

ADD_EXECUTABLE(${APP_NAME} ${SRC_FILES} ${HEADER_FILES})

TARGET_LINK_LIBRARIES(${APP_NAME} ${GLEW_LIBRARY} ${GLFW_LIBRARY} ${GLIMAGE_LIBRARY})

IF(WIN32)
	TARGET_LINK_LIBRARIES(${APP_NAME} opengl32.lib)
ELSEIF(WIN32)
	TARGET_LINK_LIBRARIES(${APP_NAME} ${GL_LIBRARY})
ENDIF(WIN32)

INSTALL(TARGETS ${APP_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
