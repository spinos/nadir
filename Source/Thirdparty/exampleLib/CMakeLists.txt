## Fri Feb 16 23:27:27 CST 2018

cmake_minimum_required (VERSION 2.8)
PROJECT (ExampleLib)
SET (CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories(".")

SET (BOOST_ROOT d:/usr/boost_1_68_0)

include_directories (${BOOST_ROOT})

file(GLOB SOURCES *.h *.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries (${PROJECT_NAME} d:/usr/boost_1_68_0/stage/lib/libboost_filesystem-vc141-mt-x64-1_68.lib)
target_link_libraries (${PROJECT_NAME} d:/usr/boost_1_68_0/stage/lib/libboost_system-vc141-mt-x64-1_68.lib)

ADD_SUBDIRECTORY ("app")
