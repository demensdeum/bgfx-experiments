cmake_minimum_required(VERSION 3.29)

set(PROJECT_NAME bgfx-2d)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_FLAGS "-s MIN_WEBGL_VERSION=2 \
-s MAX_WEBGL_VERSION=2 \
-s EXCEPTION_DEBUG \
-fexceptions \
--preload-file brick.texture.bmp \
--preload-file VertexShader.glsl \
--preload-file FragmentShader.glsl \
--bind \
--use-preload-plugins \
-Wall \
-Wextra \
-Werror=return-type \
-s USE_SDL=2 \
-s ASSERTIONS=1 \
-w \
-g4 \
-s DISABLE_EXCEPTION_CATCHING=0 \
-O3 \
--no-heap-copy \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXIT_RUNTIME=1")

include_directories("/home/demensdeum_stream/Sources/bgfx-build/bgfx/include")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

set(SOURCE_FILES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} SDL2 GL /home/demensdeum_stream/Sources/bgfx-build/bgfx/.build/wasm/bin/bgfxDebug.bc /home/demensdeum_stream/Sources/bgfx-build/bgfx/.build/wasm/bin/bxDebug.bc /home/demensdeum_stream/Sources/bgfx-build/bgfx/.build/wasm/bin/bimgDebug.bc)