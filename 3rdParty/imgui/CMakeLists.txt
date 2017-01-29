set(SRC_FILES	
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_glfw_gl3.cpp
)

set(HEADER_FILES
	imconfig.h
	imgui.h
	imgui_internal.h
	stb_rect_pack.h
	stb_textedit.h
	stb_truetype.h
	imgui_impl_glfw_gl3.h
)

add_library(imgui STATIC ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(imgui PUBLIC GLEW::glew_s glfw)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})