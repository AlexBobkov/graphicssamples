set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/common/Application.cpp
    ${PROJECT_SOURCE_DIR}/common/Camera.cpp
)

set(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/common/Application.hpp
    ${PROJECT_SOURCE_DIR}/common/Camera.hpp
)

MAKE_SAMPLE(Sample_01_1_SolidTriangle)
MAKE_SAMPLE(Sample_01_2_ColoredTriangle)
MAKE_SAMPLE(Sample_01_3_Indexes)
MAKE_SAMPLE(Sample_01_4_Map)
