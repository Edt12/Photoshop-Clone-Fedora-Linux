cmake_minimum_required(VERSION 3.31.6)
project(PhotoshopClone VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(IMGUI_SOURCES
"Dependancies/ImGui/Source/imgui_impl_glfw.cpp"
"Dependancies/ImGui/Source/imgui_impl_opengl3.cpp"
"Dependancies/ImGui/Source/imgui_tables.cpp"
"Dependancies/ImGui/Source/imgui_widgets.cpp"
"Dependancies/ImGui/Source/imgui.cpp"
"Dependancies/ImGui/Source/imgui_draw.cpp"
"Dependancies/ImGui/Source/imgui_demo.cpp"
)
message("imgui")
add_library(imgui STATIC ${IMGUI_SOURCES})
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC 
OpenGL::GL
"${CMAKE_SOURCE_DIR}/Dependancies/GLFW/libglfw3.a"
)

add_executable(PhotoshopClone 
"${CMAKE_SOURCE_DIR}/Dependancies/glad/src/glad.c"
gui.cpp 
ImageManipulator.cpp
)
target_include_directories(imgui PUBLIC 
"${CMAKE_SOURCE_DIR}/Dependancies/ImGui/Include"
"${CMAKE_SOURCE_DIR}/Dependancies/GLFW/include"
)

target_include_directories(PhotoshopClone PUBLIC
"${CMAKE_SOURCE_DIR}/Dependancies/glad/include"
"${CMAKE_SOURCE_DIR}/Dependancies/GLFW/include"
)
message("photoshop glfw")
target_link_libraries(PhotoshopClone PUBLIC "${CMAKE_SOURCE_DIR}/Dependancies/GLFW/libglfw3.a" )
target_link_libraries(PhotoshopClone PRIVATE imgui)

get_target_property(INCLUDES PhotoshopClone INCLUDE_DIRECTORIES)
message("MyApp include paths: ${INCLUDES}")


set_property(TARGET PhotoshopClone PROPERTY CXX_STANDARD 23)