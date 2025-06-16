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
"/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/GLFW/libglfw3.a"
)

add_executable(PhotoshopClone 
"/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/glad/src/glad.c"
gui.cpp 
ImageManipulator.cpp
)
target_include_directories(imgui PUBLIC 
"/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/ImGui/Include"
"/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/GLFW/include"
)

target_include_directories(PhotoshopClone PUBLIC
"/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/glad/include"
"/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/GLFW/include"
)
message("photoshop glfw")
target_link_libraries(PhotoshopClone PUBLIC "/home/ed/Documents/C++ Projects/Photoshop Clone/Dependancies/GLFW/libglfw3.a" )
target_link_libraries(PhotoshopClone PRIVATE imgui)

get_target_property(INCLUDES PhotoshopClone INCLUDE_DIRECTORIES)
message("MyApp include paths: ${INCLUDES}")


set_property(TARGET PhotoshopClone PROPERTY CXX_STANDARD 23)