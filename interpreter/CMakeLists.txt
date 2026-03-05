cmake_minimum_required(VERSION 3.16)

message(STATUS "===========================")
message(STATUS "Configuring interpreter CLI")
message(STATUS "===========================")

# project declaration
project(INTERPRETER_CLI VERSION "${INTERPRETER_MAJOR}.${INTERPRETER_PATCH}")
project(INTERPRETER_CLI DESCRIPTION "A CLI for ALE's interpreter")

# needed includes to make CMake work
include(CheckCXXCompilerFlag)
# installation directories (platform dependent)
include(GNUInstallDirs)

# **********************************************************************
# Custom functions

function(configure_interpreter_executable executable)
	add_dependencies(${executable} ${INTERPRETER_LIB_NAME})
	
	# Interpreter lib
	target_link_libraries(${executable} ${INTERPRETER_LIB_DIR}/lib${INTERPRETER_LIB_NAME}.so)
	target_link_libraries(${executable} ${ALE_LIB_DIR}/lib${ALE_LIB_NAME}.so)

	configure_executable(${executable})

endfunction(configure_interpreter_executable)

# **********************************************************************
# MAKE EXECUTABLE

add_executable(interpreter main.cpp)
configure_interpreter_executable(interpreter)
