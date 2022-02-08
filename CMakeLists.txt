cmake_minimum_required(VERSION 2.8)
project(libxco)
set(LIBXCO_VERSION 0.1)

# Include cmake util.
include(util.cmake)

# Set config.
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAAGS} -O2 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# Add static and shared library target.
file(GLOB libx_srcs ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(xco_static STATIC ${libx_srcs})
add_library(xco_shared SHARED ${libx_srcs})
set_target_properties(xco_static PROPERTIES OUTPUT_NAME xco)
set_target_properties(xco_shared PROPERTIES OUTPUT_NAME xco VERSION ${LIBXCO_VERSION} SOVERSION ${LIBXCO_VERSION})
RedefineFileMacro(xco_static)
RedefineFileMacro(xco_shared)

# Set install.
install(TARGETS xco_static LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
install(TARGETS xco_shared LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION "include/libx" FILES_MATCHING PATTERN "*.h")

# Add test target.
include_directories(${PROJECT_SOURCE_DIR}/src)
set(libs dl xco_static)
file(GLOB src_files ${PROJECT_SOURCE_DIR}/test/*.cpp)
foreach(src_file ${src_files})
    STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" target ${src_file})
    add_executable(${target} ${src_file})
    target_link_libraries(${target} ${libs})
    RedefineFileMacro(${target})
endforeach(src_file)