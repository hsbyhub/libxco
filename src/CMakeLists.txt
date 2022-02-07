cmake_minimum_required(VERSION 2.8)
project(libx)
set(LIBX_VERSION 0.1)

# Include cmake util.
include(${PROJECT_SOURCE_DIR}/util.cmake)

# Set config.
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAAGS} -O2 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# Add static and shared library target.
Set(libx_srcs)
file(GLOB libx_srcs ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(x_static STATIC ${libx_srcs})
#add_library(x_shared SHARED ${libx_srcs})
set_target_properties(x_static PROPERTIES OUTPUT_NAME xlib)
#set_target_properties(x_shared PROPERTIES OUTPUT_NAME xlib VERSION ${LIBX_VERSION} SOVERSION ${LIBX_VERSION})
RedefineFileMacro(x_static)
#RedefineFileMacro(x_shared)

# Set install.
install(TARGETS x_static LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
#install(TARGETS x_shared LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION "include/libx" FILES_MATCHING PATTERN "*.h")

# Add test target.
set(libs dl x_static)
file(GLOB src_files ${PROJECT_SOURCE_DIR}/../test/*.cpp)
foreach(src_file ${src_files})
    STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" target ${src_file})
    MESSAGE(${target})
    add_executable(${target} ${src_file})
    target_link_libraries(${target} ${libs})
    RedefineFileMacro(${target})
endforeach(src_file)