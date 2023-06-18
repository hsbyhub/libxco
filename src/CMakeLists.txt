cmake_minimum_required(VERSION 2.8)
project(libxco)
set(LIBXCO_VERSION 0.1)

# Set config.
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build/bin)
set(LIBRARY_OUTPUT_PATH /tmp/libxco_lib)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAAGS} -O3 -g -std=c++11 -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# Use c and asm.
enable_language(C ASM)

# Add static and shared library target.
set(libs dl)
file(GLOB libxco_srcs
        ${PROJECT_SOURCE_DIR}/*.cpp
        ${PROJECT_SOURCE_DIR}/http/*.cpp
        ${PROJECT_SOURCE_DIR}/*.S)
add_library(xco_static STATIC ${libxco_srcs})
add_library(xco_shared SHARED ${libxco_srcs})
target_link_libraries(xco_static ${libs})
target_link_libraries(xco_shared ${libs})
set_target_properties(xco_static PROPERTIES OUTPUT_NAME xco)
set_target_properties(xco_shared PROPERTIES OUTPUT_NAME xco VERSION ${LIBXCO_VERSION} SOVERSION ${LIBXCO_VERSION})

# Set install.
#install(TARGETS xco_static LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
install(TARGETS xco_shared LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
install(DIRECTORY . ./http DESTINATION "include/xco" FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ./util DESTINATION "include/xco" FILES_MATCHING PATTERN "*.hpp")

# Add test target.
include_directories(${PROJECT_SOURCE_DIR})
set(libs dl xco_static)
file(GLOB src_files ${PROJECT_SOURCE_DIR}/../test/*.cpp)
foreach(src_file ${src_files})
    STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" target ${src_file})
    add_executable(${target} ${src_file})
    target_link_libraries(${target} ${libs})
endforeach(src_file)