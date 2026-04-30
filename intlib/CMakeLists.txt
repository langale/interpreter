cmake_minimum_required(VERSION 3.16)

message(STATUS "===============================")
message(STATUS "Configuring interpreter library")
message(STATUS "===============================")

project(INTERPRETER_LIB VERSION "${INTERPRETER_VERSION}")
project(INTERPRETER_LIB DESCRIPTION "Interpreter for the ALE language")

message(STATUS "    INTERPRETER_DIR:      ${INTERPRETER_DIR}")
message(STATUS "    INTERPRETER_LIB_DIR:  ${INTERPRETER_LIB_DIR}")
message(STATUS "    INTERPRETER_LIB_NAME: ${INTERPRETER_LIB_NAME}")

# **********************************************************************

set(TargetStringLibrary "${INTERPRETER_LIB_NAME}")

add_library(${TargetStringLibrary} SHARED "")

set_target_properties(${TargetStringLibrary} PROPERTIES VERSION "${INTERPRETER_VERSION}")
set_target_properties(${TargetStringLibrary} PROPERTIES SOVERSION 1)
set_target_properties(${TargetStringLibrary} PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB_RECURSE SOURCES ${INTERPRETER_DIR}/intlib/*)
target_sources(${TargetStringLibrary} PRIVATE ${SOURCES})

# **********************************************************************

configure_object(${TargetStringLibrary})
