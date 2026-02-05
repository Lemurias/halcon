###############################################################################
#                            USER SETTINGS (BEGIN)
###############################################################################

#######################################
#             APP SETTINGS
#######################################

# Application main file name
if(NOT DEFINED TARGET_FILE)
    set(TARGET_FILE ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)
endif()

# Executable file name
if(NOT DEFINED EXE_NAME)
    set(EXE_NAME lpf_sim)
endif()

#######################################
#       MODULES STATIC LIBRARY
#######################################

# Base directory
if(NOT DEFINED EXTERNAL_DIR)
    set(EXTERNAL_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib/modules/)
endif()

# Modules
if(NOT DEFINED MODULES_LIST)
    set(MODULES_LIST

    )
endif()

#######################################
#       LOCAL STATIC LIBRARY
#######################################

# Base directory
if(NOT DEFINED LOCAL_DIR)
    set(LOCAL_DIR ${CMAKE_CURRENT_LIST_DIR}/src/)
endif()

# Modules
if(NOT DEFINED LOCAL_LIST)
    set(LOCAL_LIST
        root
        filter
        sin_generator
        adder
    )
endif()

#######################################
#         HALCON CORE (RELEASE)
#######################################

if(NOT DEFINED CORE_DIR)
    set(CORE_DIR ${CMAKE_CURRENT_LIST_DIR}/../lib/core/)
endif()

if(NOT DEFINED CORE_TYPE)
    set(CORE_TYPE "FULL")
endif()

#######################################
#             RUN PATH
#######################################

if(NOT DEFINED INSTALL_DIR)
    set(INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/run/)
endif()

###############################################################################
#                            USER SETTINGS (END)
###############################################################################

###############################################################################
#                          PROJECT COMPILATION (BEGIN)
###############################################################################

#######################################
#            HALCON BANNER
#######################################

set(BANNER
"

▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
██ SIMULATOR POWERED BY ... ████████████████████████████████████████████████████
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
#           PROJECT NAME
#######################################

project(${EXE_NAME})

#######################################
#   CMAKE AND GCC COMPILATION FLAGS
#######################################

# CMake version
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -W -Werror -Wsign-compare -Wno-misleading-indentation -ftree-vectorize")

#######################################
#   CONDITIONAL COMPILATION MACROS
#######################################

if(CORE_TYPE STREQUAL "FULL")
    add_definitions(-D_RUN_POSEDGE_LOGIC_ONLY=0)
else()
    add_definitions(-D_RUN_POSEDGE_LOGIC_ONLY=1)
endif()

#######################################
#         CORE PATH SETTINGS
#######################################

set(CORE_INC_DIR ${CORE_DIR}inc/)
set(CORE_BIN_DIR ${CORE_DIR}src/)

set(CORE_FULL_BIN lib_halcon_core.a)
set(CORE_POSEDGE_BIN lib_halcon_core_only_posedge.a)

set(CORE_DEPENDENCES
    yaml-cpp
    Boost::boost
    fftw3
)

find_package(Boost 1.71 REQUIRED)
find_library(fftw3 /usr/include/fftw3.h)

#######################################
#       CORE VERSION SELECTION
#######################################

if(CORE_TYPE STREQUAL "FULL")
    set(CORE_LIB ${CORE_BIN_DIR}${CORE_FULL_BIN})
    message("-- Loading FULL HALCON core")
else()
    set(CORE_LIB ${CORE_BIN_DIR}${CORE_POSEDGE_BIN})
    message("-- Loading POSEDGE HALCON core")
endif()

#######################################
#       EXTERNAL STATIC LIBRARY
#######################################

list(LENGTH MODULES_LIST MODULES_LEN)

if(${MODULES_LEN} GREATER 0)

    foreach(MODULE ${MODULES_LIST})

        message("-- Loading external module ${MODULE} from ${EXTERNAL_DIR}${MODULE}")

        # C++ support    
        file(GLOB TMP_SRC ${EXTERNAL_DIR}${MODULE}/*.cpp)
        list(APPEND MODULES_SRC ${TMP_SRC})

        file(GLOB TMP_HEADER ${EXTERNAL_DIR}${MODULE}/*.hpp)
        list(APPEND MODULES_HEADER ${TMP_HEADER})

        # C support
        file(GLOB TMP_SRC ${EXTERNAL_DIR}${MODULE}/*.c)
        list(APPEND MODULES_SRC ${TMP_SRC})

        file(GLOB TMP_HEADER ${EXTERNAL_DIR}${MODULE}/*.h)
        list(APPEND MODULES_HEADER ${TMP_HEADER})
        
        # Include directories
        list(APPEND MODULES_INCLUDE_DIRS ${EXTERNAL_DIR}${MODULE})

    endforeach()

    add_library(external STATIC ${MODULES_SRC} ${MODULES_HEADER})

    list(APPEND MODULES_INCLUDE_DIRS ${CORE_INC_DIR})

    target_include_directories(external PUBLIC ${MODULES_INCLUDE_DIRS})

endif()

#######################################
#        LOCAL STATIC LIBRARY
#######################################

message("-- Loading POSEDGE HALCON core")

foreach(MODULE ${LOCAL_LIST})

    message("-- Loading local module ${MODULE} from ${LOCAL_DIR}${MODULE}")

    # C++ support    
    file(GLOB TMP_SRC ${LOCAL_DIR}${MODULE}/*.cpp)
    list(APPEND LOCAL_SRC ${TMP_SRC})

    file(GLOB TMP_HEADER ${LOCAL_DIR}${MODULE}/*.hpp)
    list(APPEND LOCAL_HEADER ${TMP_HEADER})

    # C support
    file(GLOB TMP_SRC ${LOCAL_DIR}${MODULE}/*.c)
    list(APPEND LOCAL_SRC ${TMP_SRC})

    file(GLOB TMP_HEADER ${LOCAL_DIR}${MODULE}/*.h)
    list(APPEND LOCAL_HEADER ${TMP_HEADER})

    # Include directories
    list(APPEND LOCAL_INCLUDE_DIRS ${LOCAL_DIR}${MODULE})

endforeach()

add_library(local STATIC ${LOCAL_SRC} ${LOCAL_HEADER})

list(APPEND LOCAL_INCLUDE_DIRS ${CORE_INC_DIR})

if(${MODULES_LEN} GREATER 0)
    list(APPEND LOCAL_INCLUDE_DIRS ${MODULES_INCLUDE_DIRS})
endif()

target_include_directories(local PUBLIC ${LOCAL_INCLUDE_DIRS})

#######################################
#        APPLICATION COMPILING
#######################################

add_executable(${EXE_NAME} ${TARGET_FILE})

#######################################
#        APPLICATION LINKING
#######################################

set(EXE_LIBS
    local
    ${CORE_LIB}
    ${CORE_DEPENDENCES}
)

if(${MODULES_LEN} GREATER 0)

    list(APPEND EXE_LIBS external)

endif()

set_target_properties(${EXE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${EXE_NAME} PUBLIC ${EXE_LIBS})

#######################################
#      APPLICATION INSTALLATION
#######################################

install(TARGETS ${EXE_NAME} DESTINATION ${INSTALL_DIR})

###############################################################################
#                          PROJECT COMPILATION (END)
###############################################################################