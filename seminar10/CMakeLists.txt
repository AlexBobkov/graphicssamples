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
    
    shaders10/cull.vert
    shaders10/cull.geom
    
    shaders10/tess.vert
    shaders10/tess.control
    shaders10/tess.eval
    shaders10/tess.frag
)

source_group("Shaders" FILES
    ${SHADER_FILES}
)

MAKE_SAMPLE(Sample_10_1_ComputeShader)
MAKE_SAMPLE(Sample_10_2_GPUCulling)
MAKE_SAMPLE(Sample_10_3_Tesselation)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar10/shaders10 DESTINATION ${CMAKE_INSTALL_PREFIX})