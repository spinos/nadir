## Fri Feb 16 23:27:27 CST 2018

cmake_minimum_required (VERSION 2.8)
PROJECT (ExampleLib)
SET (CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories(".")

SET (BOOST_ROOT d:/usr/boost_1_68_0)

include_directories (${BOOST_ROOT})

SET (HDF5_ROOT d:/usr/hdf5-1.8.21-s)

include_directories (${HDF5_ROOT}/include)

#ADD_DEFINITIONS (-DH5_BUILT_AS_DYNAMIC_LIB)

file(GLOB SOURCES *.h *.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries (${PROJECT_NAME} ${BOOST_ROOT}/stage/lib/libboost_filesystem-vc141-mt-x64-1_68.lib)
target_link_libraries (${PROJECT_NAME} ${BOOST_ROOT}/stage/lib/libboost_system-vc141-mt-x64-1_68.lib)
target_link_libraries (${PROJECT_NAME} ${HDF5_ROOT}/lib/libhdf5.lib)
target_link_libraries (${PROJECT_NAME} D:/usr/lib/libszip.lib)
target_link_libraries (${PROJECT_NAME} D:/usr/lib/libzlib.lib)

set_target_properties( ${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 )

ADD_SUBDIRECTORY ("app")
