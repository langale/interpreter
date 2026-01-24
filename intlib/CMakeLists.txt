cmake_minimum_required(VERSION 3.16)

message(STATUS "===============================")
message(STATUS "Configuring interpreter library")
message(STATUS "===============================")

# needed includes to make CMake work
include(CheckCXXCompilerFlag)
# installation directories (platform dependent)
include(GNUInstallDirs)

# project declaration
project(INTERPRETER_LIB VERSION "${INTERPRETER_MAJOR}.${INTERPRETER_PATCH}")
project(INTERPRETER_LIB DESCRIPTION "Interpreter for the ALE language")

message(STATUS "Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "Target library: ${INTERPRETER_LIB_NAME}")

set(TargetStringLibrary "${INTERPRETER_LIB_NAME}")

# **********************************************************************
# **********************************************************************

# declare library
add_library(${TargetStringLibrary} SHARED "")

# define library's properties
set_target_properties(${TargetStringLibrary} PROPERTIES VERSION "${INTERPRETER_MAJOR}.${INTERPRETER_PATCH}")
set_target_properties(${TargetStringLibrary} PROPERTIES SOVERSION 1)
set_target_properties(${TargetStringLibrary} PROPERTIES LINKER_LANGUAGE CXX)

# **********************************************************************
# HEADER AND SOURCE FILES FOR COMPILATION

file(GLOB_RECURSE SOURCES ${INTERPRETER_DIR}/intlib/*)
target_sources(${TargetStringLibrary} PRIVATE ${SOURCES})

# **********************************************************************
# CONFIGURE

configure_executable(${TargetStringLibrary})
