################################################################################
# ██████████████████████████████████████████████████████████████████████████████
# █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
# █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
# █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
# █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
# █    ███    ██▌          ██▌   ███████▄   ▀██▀   ███    ███    ███   ██▄     █
# █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
# ██████████████████████████████████████████████████████████████████████████████
# █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
# ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
################################################################################
# Author: Patricio Reus Merlo
# Date: 05/21/2024
################################################################################
# MIT License
# 
# Copyright (c) 2024 Fundacion Fulgor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

################################################################################
#                            USER SETTINGS (BEGIN)
################################################################################

#######################################
# HALCON BANNER
#######################################

set(BANNER
"

████████████████████████████████████████████████████████████████████████████████
██▀▀▀█████▀▀▀███████▀▀▀███████▀▀███████████▀▀▀▀▀▀████████▀▀▀▀██████▀▀▀█████▀▀▀██
██░░░░███▌░░░█████▌░░░░░█████░░░░████████▀ ░░░░░░░▀███▀░░░▄▄░░░▀██▌░░░░▀███░░░██
██░░░░███▒░░░█████░░▐░░░▐████░░░░███████ ░░░████▄▄▄██░░░░████░░░░█▌░░░░░ ▀█░░░██
██░░░░░░░░░░░████░░░██░░░▀███░░░░███████░░░░█████████░░░░████░░░░█▌░░░▄░░░▀░░░██
██░░░░███▒░░░███░░░░░░░░░░███░░░░███████ ░░░████▀▀▀██░░░░████░░░░█▌░░░██ ░░░░░██
██░░░░███▌░░░██▀░░▐████░░░░██░░░░░░░░░▒██▄░░░░░░░░▄███▄░░░▀▀░░░▄██▌░░░███▄░░░░██
██▄▄▄█████▄▄▄██▄▄▄██████▄▄▄██▄▄▄▄▄▄▄▄▄██████▄▄▄▄█████████▄▄▄▄██████▄▄▄█████▄▄▄██
██████████████████████████ DSP SIMULATION ENGINE ███████████████████████████████
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀

"
)

message(${BANNER})

#######################################
# LIBRARY NAMES
#######################################

# Project name
project(HALCON)

# Library base name
set(LIB_NAME core)

# File names
set(CORE_FULL_BIN "_halcon_core")
set(CORE_POSEDGE_BIN "_halcon_core_only_posedge")

#######################################
# LIBRARY SRC
#######################################

# Base directory
set(CORE_DIR src)

# Modules
set(CORE_MODULES
        clock
        command
        command_handler
        command_line_parser
        fixed_point
        handler
        halcon
        logger
        module
        nameable
        port
        reflection
        register
        scheduler
        setter
        simulator
        tictoc
)

################################################################################
# USER SETTINGS (END)
################################################################################

################################################################################
# PROJECT COMPILATION (BEGIN)
################################################################################

#######################################
# CONDITIONAL COMPILATION
#######################################

# MODE: 1:Posedge only - 0:Posedge and negedge
option(RUN_POSEDGE_LOGIC_ONLY "Enable Posedge Only" 0)

if(RUN_POSEDGE_LOGIC_ONLY EQUAL 0)
    message("-- Scheduler and Clock supports POSEDGE AND NEGEDGE LOGIC")
elseif(RUN_POSEDGE_LOGIC_ONLY EQUAL 1)
    message("-- Scheduler and Clock supports ONLY POSEDGE LOGIC")
endif()

#######################################
# CMAKE AND GXX COMPILATION FLAGS
#######################################

# CMake version
cmake_minimum_required(VERSION 3.10)

# Use g++ > 13
set(CMAKE_CXX_COMPILER g++-13)

# Compilation type
set(CMAKE_BUILD_TYPE Release)

# Use the C++20 standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Enable all standard warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W")

# Treat all warnings as errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

# Warn about comparisons between signed and unsigned values
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")

# Disable misleading indentation warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-misleading-indentation")

# Enable vectorization of loops
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftree-vectorize")

# Enable most warning messages
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Enable extra warning messages
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")

# Issue all the warnings demanded by strict ISO C and ISO C++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")

# Warn whenever a local variable shadows another variable
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")

# Warn if testing floating point values for equality
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")

# Enable format string warnings (with extra checks)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")

# Warn for implicit type conversions that may alter a value
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")

# Warn about suspicious uses of logical operators
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")

# Warn if an include directory does not exist
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs")

# Warn if a null dereference is detected
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference")

# Warn if string literals are longer than the maximum length
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverlength-strings")

# Warn on anything that is unused
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")

# Warn if a variable is used without being initialized
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized")

# Warn if a variable might be used uninitialized
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmaybe-uninitialized")

# Warn if a switch statement does not cover all enum values
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")

# Strict aliasing rules (level 2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing=2")

# Warn if a float is promoted to a double
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")

# Warn about implicit fallthrough in switch statements
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough")

# Warn if anything is declared more than once
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")

# Optimize for the host architecture
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

# Enable stack protection
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")

# Enable additional compile-time and run-time checks
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2")

# Conditional flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")

    message("-- Build as Release")

    # Optimization level 3 (higher optimization)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

else()

    message("-- Build as Debug")

    # Optimization level 0 (lower optimization)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

    # Generate debug information
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

    # Do not omit the frame pointer (useful for debugging)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")

    # Disable inline expansion for easier debugging
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline")

endif()

#######################################
# MACROS
#######################################

add_definitions(-D_RUN_POSEDGE_LOGIC_ONLY=${RUN_POSEDGE_LOGIC_ONLY})

#######################################
# DEPENDENCES
#######################################

find_package(spdlog REQUIRED)
find_package(Boost 1.71 REQUIRED)
find_library(fftw3 /usr/include/fftw3.h)

#######################################
# STATIC LIBRARY
#######################################

foreach(MODULE ${CORE_MODULES})

    message("-- Loading ${MODULE}")

    # C++ support    
    file(GLOB_RECURSE TMP_SRC ${PROJECT_SOURCE_DIR}/${CORE_DIR}/${MODULE}/*.cpp)
    list(APPEND CORE_SRC ${TMP_SRC})

    file(GLOB_RECURSE TMP_HEADER ${PROJECT_SOURCE_DIR}/${CORE_DIR}/${MODULE}/*.hpp)
    list(APPEND CORE_HEADER ${TMP_HEADER})

    # C support
    file(GLOB_RECURSE TMP_SRC ${PROJECT_SOURCE_DIR}/${CORE_DIR}/${MODULE}/*.c)
    list(APPEND CORE_SRC ${TMP_SRC})

    file(GLOB_RECURSE TMP_HEADER ${PROJECT_SOURCE_DIR}/${CORE_DIR}/${MODULE}/*.h)
    list(APPEND CORE_HEADER ${TMP_HEADER})

    # Include directories
    list(APPEND CORE_INCLUDE_DIRS ${CORE_DIR}/${MODULE})

endforeach()

#######################################
# APPLICATION COMPILING
#######################################

add_library(${LIB_NAME} STATIC ${CORE_SRC} ${CORE_HEADER})

target_include_directories(${LIB_NAME} PUBLIC ${CORE_INCLUDE_DIRS})

#######################################
# APPLICATION LINKING
#######################################

set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LIB_NAME} PUBLIC yaml-cpp Boost::boost fftw3)

#######################################
# LIBRARY NAMING
#######################################

if(RUN_POSEDGE_LOGIC_ONLY EQUAL 0)
    set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME ${CORE_FULL_BIN})
else()
    set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME ${CORE_POSEDGE_BIN})
endif()

################################################################################
# PROJECT COMPILATION (END)
################################################################################