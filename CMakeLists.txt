#
# PROJECT, LIBRARY AND BINARY NAMES
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(relish_unit_tests)
SET(EXECUTABLE_NAME relishd)
SET(LIBRARY_NAME relish)


#
# COMPILER CONFIGURATION
#

# Use C++ 11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -v")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v")

# To use DEBUG
IF (DEFINED ENV{DEBUG})
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF (DEFINED ENV{DEBUG})

#
# INCLUDE DIRECTORIES
#

INCLUDE_DIRECTORIES(./src)

#
# SOURCE CODE AND TEST SOURCE CODE
#

FILE(
  GLOB_RECURSE
  SRC_FILES
  ./src/*
  )

# FILE(
#   GLOB_RECURSE
#   TEST_SRC_FILES
#   ./tests/*
#   )

#
# GMOCK AND OTHER DEPENDENCIES
#

# IF (UNIX)
#   FIND_LIBRARY(
#     DL
#     NAMES
#     dl
#     )
# ENDIF (UNIX)

#
# CREATE LIBRARY
#

ADD_LIBRARY(
  ${LIBRARY_NAME}
  ${SRC_FILES}
  )

#
# CREATE EXECUTABLE
#

ADD_EXECUTABLE(
  ${EXECUTABLE_NAME}
  ${SRC_FILES}
  )

#
# LINK LIBRARIES AND EXECUTABLE
#

TARGET_LINK_LIBRARIES(
  ${EXECUTABLE_NAME}
  ${LIBRARY_NAME}
  ${DL}
  )
