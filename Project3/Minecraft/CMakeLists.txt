cmake_minimum_required(VERSION 3.10)
project(MinecraftClone)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE MINECRAFT_CLONE_SRC_CODE
    "src/*.cpp"
    "${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c"
    "${CMAKE_SOURCE_DIR}/dependencies/stb/stb/stb_image.c"
)

# OPENGL LIB
add_executable(minecraft_fiver "${MINECRAFT_CLONE_SRC_CODE}")

if (WIN32)
    target_link_libraries(minecraft_fiver PUBLIC user32 kernel32 opengl32)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(minecraft_fiver ${OPENGL_gl_LIBRARY} X11)
endif()

# Non platform specific libaries.
target_include_directories(minecraft_fiver PUBLIC
    "${CMAKE_SOURCE_DIR}/dependencies/glad/include/"
    "${CMAKE_SOURCE_DIR}/dependencies/stb/"
)
