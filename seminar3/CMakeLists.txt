set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/common/Application.cpp
    ${PROJECT_SOURCE_DIR}/common/Camera.cpp
    ${PROJECT_SOURCE_DIR}/common/Mesh.cpp
    ${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
)

set(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/common/Application.hpp
    ${PROJECT_SOURCE_DIR}/common/Camera.hpp
    ${PROJECT_SOURCE_DIR}/common/Mesh.hpp
    ${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
)

set(SHADER_FILES
    shaders3/shaderUBO.vert
    shaders3/shader.frag
)

source_group("Shaders" FILES
    ${SHADER_FILES}
)

MAKE_SAMPLE(Sample_03_5_UBO)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar3/shaders3 DESTINATION ${CMAKE_INSTALL_PREFIX})
