cmake_minimum_required(VERSION 3.16)

message(STATUS "===========================")
message(STATUS "Configuring interpreter CLI")
message(STATUS "===========================")

project(ALE_INTERPRETER_CLI VERSION "${INTERPRETER_VERSION}")
project(ALE_INTERPRETER_CLI DESCRIPTION "A CLI for ALE's interpreter")

set(INTERPRETER_EXE_DIR ${INTERPRETER_DIR}/${BUILD_TYPE_DIR}/interpreter)
message(STATUS "    INTERPRETER_EXE_DIR:  ${INTERPRETER_EXE_DIR}")
message(STATUS "    INTERPRETER_LIB_DIR:  ${INTERPRETER_LIB_DIR}")
message(STATUS "    INTERPRETER_LIB_NAME: ${INTERPRETER_LIB_NAME}")

# **********************************************************************
# Custom functions

function(configure_interpreter executable)
	add_dependencies(${executable} ${INTERPRETER_LIB_NAME})
	
	# interpreter library (intlib)
	target_link_libraries(${executable} ${INTERPRETER_LIB_DIR}/lib${INTERPRETER_LIB_NAME}.so)

	configure_object(${executable})

endfunction(configure_interpreter)

# **********************************************************************
# MAKE EXECUTABLE

add_executable(interpreter main.cpp)

set_target_properties(interpreter PROPERTIES VERSION "${INTERPRETER_VERSION}")
set_target_properties(interpreter PROPERTIES SOVERSION 1)
set_target_properties(interpreter PROPERTIES LINKER_LANGUAGE CXX)

configure_interpreter(interpreter)
