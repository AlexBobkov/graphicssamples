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
    shaders10/particleWithComputeShader.vert
    shaders10/particleWithComputeShader.frag
    shaders10/particleWithComputeShader.compute
)

source_group("Shaders" FILES
    ${SHADER_FILES}
)

MAKE_SAMPLE(Sample_10_5_ComputeShader)
MAKE_SAMPLE(Sample_10_6_GPUCulling)
MAKE_SAMPLE(Sample_10_7_Tesselation)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar10/shaders10 DESTINATION ${CMAKE_INSTALL_PREFIX})