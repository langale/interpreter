cmake_minimum_required(VERSION 3.16)
project(aleinterpreter LANGUAGES CXX)

# Turn on export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project's version
set(INTERPRETER_MAJOR 99.99)
set(INTERPRETER_PATCH 99)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

# ******************************************************************************
# Custom functions

function(add_compile_flag executable flag)
	check_cxx_compiler_flag(${flag} CXX_HAS_FLAG_${flag})
	if (CXX_HAS_FLAG_${flag})
		target_compile_options(${executable} PRIVATE ${flag})
	endif()
endfunction(add_compile_flag)

function(add_link_flag executable flag)
	check_cxx_compiler_flag(${flag} CXX_HAS_FLAG_${flag})
	if (CXX_HAS_FLAG_${flag})
		target_link_options(${executable} PRIVATE ${flag})
	endif()
endfunction(add_link_flag)

function(define_symbol executable symbol)
	target_compile_options(${executable} PRIVATE ${symbol})
endfunction(define_symbol)

function(configure_executable executable)
	# define executable's properties
	set_target_properties(${executable} PROPERTIES SOVERSION 1)
	set_target_properties(${executable} PROPERTIES LINKER_LANGUAGE CXX)

	# ******************************************************************************
	# Definitions

	# ******************************************************************************
	# Warning flags

	add_compile_flag(${executable} -Wall)
	add_compile_flag(${executable} -Wextra)
	add_compile_flag(${executable} -Wshadow)
	add_compile_flag(${executable} -Wnon-virtual-dtor)
	add_compile_flag(${executable} -Wold-style-cast)
	add_compile_flag(${executable} -Wcast-align)
	add_compile_flag(${executable} -Wunused)
	add_compile_flag(${executable} -Woverloaded-virtual)
	add_compile_flag(${executable} -Wpedantic)
	add_compile_flag(${executable} -Wconversion)
	add_compile_flag(${executable} -Wsign-conversion)
	add_compile_flag(${executable} -Wnull-dereference)
	add_compile_flag(${executable} -Wdouble-promotion)
	#add_compile_flag(${executable} -Wformat=2)
	add_compile_flag(${executable} -Wduplicated-cond)
	add_compile_flag(${executable} -Wduplicated-branches)
	add_compile_flag(${executable} -Wlogical-op)
	add_compile_flag(${executable} -Wuseless-cast)
	add_compile_flag(${executable} -Wrestrict)

	# ******************************************************************************
	# Compile-mode dependent flags

	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		add_compile_flag(${executable} -Og)
		define_symbol(${executable} -DDEBUG)
		define_symbol(${executable} -D_GLIBCXX_DEBUG)
	elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Profiling")
		add_compile_flag(${executable} -g)
		add_compile_flag(${executable} -pg)
		add_compile_flag(${executable} -fno-omit-frame-pointer)
	else()
		# flags added by default:
		#   -O3
		#   -DNDEBUG
		define_symbol(${executable} -UDEBUG)

		if("${IPO_IS_SUPPORTED}" STREQUAL "YES")
			set_property(TARGET ${executable} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
		endif()
	endif()

	if (ENABLE_ASAN)
		add_compile_flag(${executable} -fno-omit-frame-pointer)
		target_compile_options(${executable} PRIVATE -fsanitize=address)
		target_link_options(${executable} PRIVATE -fsanitize=address)
	endif()

	# ***************************************************************************
	# Include paths

	target_include_directories(${executable} PRIVATE ${ALE_DIR})
	target_include_directories(${executable} PRIVATE ${INTERPRETER_DIR})

endfunction(configure_executable)

# ******************************************************************************
# Set default build type

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(
		CMAKE_BUILD_TYPE "${default_build_type}"
		CACHE STRING "Choose the type of build."
		FORCE
	)
	# Set the possible values of build type for cmake-gui
	set_property(
		CACHE CMAKE_BUILD_TYPE
		PROPERTY
			STRINGS "Debug" "Release"
	)
endif()

# ******************************************************************************
# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ******************************************************************************
# Path to the current CMakeFile

set(INTERPRETER_DIR ${CMAKE_SOURCE_DIR})
message(STATUS "INTERPRETER_DIR: ${INTERPRETER_DIR}")

# ******************************************************************************

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message(STATUS "Link against intlib (debug compilation of intlib)")
	set(INTERPRETER_LIB_DIR ${INTERPRETER_DIR}/cmake-build-debug/intlib)
	set(INTERPRETER_LIB_NAME intlib)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Profiling")
	message(STATUS "Link against intlibprofiling (debug compilation of intlibprofiling)")
	set(INTERPRETER_LIB_DIR ${INTERPRETER_DIR}/cmake-build-profiling/intlib)
	set(INTERPRETER_LIB_NAME intlibprofiling)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Instrumentation")
	message(STATUS "Link against intlibinstrumentation (debug compilation of intlibinstrumentation)")
	set(INTERPRETER_LIB_DIR ${INTERPRETER_DIR}/cmake-build-instrumentation/intlib)
	set(INTERPRETER_LIB_NAME intlibinstrumentation)
else()
	message(STATUS "Link against intliboptimized (debug compilation of intliboptimized)")
	set(INTERPRETER_LIB_DIR ${INTERPRETER_DIR}/cmake-build-release/intlib)
	set(INTERPRETER_LIB_NAME intliboptimized)
endif()
message(STATUS "    INTERPRETER_LIB_DIR:  ${INTERPRETER_LIB_DIR}")
message(STATUS "    INTERPRETER_LIB_NAME: ${INTERPRETER_LIB_NAME}")

# *******************************************************************************
# External libraries

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Looking for ALE
if (NOT DEFINED ALE_DIR)
	message(STATUS "ALE's development root directory was not given.")
	message(STATUS "    Note that even if the library is installed in the system, this interpreter")
	message(STATUS "    use 'private' headers of the library, which are not distributed.")
	message(STATUS "    Please, rerun 'cmake' with the location of ALE's development root directory:")
	message(STATUS "        -DALE_DIR=/path/to/ale")
	message(STATUS "    Make sure it is an absolute path!")
	message(FATAL_ERROR "Please, read the last few lines above and follow the instructions given.")
else()
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(ALE_LIB_DIR ${ALE_DIR}/cmake-build-debug)
		set(ALE_LIB_NAME ale)
	elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Profiling")
		set(ALE_LIB_DIR ${ALE_DIR}/cmake-build-profiling)
		set(ALE_LIB_NAME aleprofiling)
	elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Instrumentation")
		set(ALE_LIB_DIR ${ALE_DIR}/cmake-build-instrumentation)
		set(ALE_LIB_NAME aleinstrumentation)
	else()
		set(ALE_LIB_DIR cmake-build-release)
		set(ALE_LIB_NAME aleoptimized)
	endif()
	
	message(STATUS "Compile against ALE:")
	message(STATUS "    ALE_DIR:      ${ALE_DIR}")
	message(STATUS "    ALE_LIB_DIR:  ${ALE_LIB_DIR}")
	message(STATUS "    ALE_LIB_NAME: ${ALE_LIB_NAME}")
endif()

# *******************************************************************************
# Build-dependent options

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Profiling")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Instrumentation")
else()
	# IPO: Inter-Procedural Optimization
	include(CheckIPOSupported)
	check_ipo_supported(RESULT IPO_IS_SUPPORTED OUTPUT IPO_CHECK_ERROR)
	message(STATUS "IPO_IS_SUPPORTED='${IPO_IS_SUPPORTED}'")
	if("${IPO_IS_SUPPORTED}" STREQUAL "YES")
		message(STATUS "    IPO is supported.")
	else()
		message(STATUS "    IPO is not supported.")
		message(STATUS "    Error message: ${IPO_CHECK_ERROR}")
	endif()
endif()

# *******************************************************************************
# Add subdirectories

add_subdirectory(intlib)
add_subdirectory(interpreter)
