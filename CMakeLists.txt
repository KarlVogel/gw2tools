cmake_minimum_required(VERSION 2.8)

IF (NOT WIN32)
  SET (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CXXFLAGS} ${CMAKE_CXX_FLAGS} -std=c++0x -ggdb3 -g -m32")
  SET (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -ggdb3 -g -m32")
ENDIF (NOT WIN32)

find_library(bfd_LIBRARY NAMES bfd
             PATHS /usr/local/lib /usr/lib
)
find_path(bfd_INCLUDE_DIR bfd.h
  /usr/local/include
  /usr/include
)

add_executable(packet-extract packet-extract.cpp)
include_directories(packet-extract ${bfd_INCLUDE_DIR})
target_link_libraries(packet-extract ${bfd_LIBRARY} ${CMAKE_DL_LIBS})

