#!/usr/bin/python3
################################################################################
# ██████████████████████████████████████████████████████████████████████████████
# █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
# █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
# █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
# █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
# █    ███    ██▌          ██▌   ███████▄   ▀██▀   ▐██    ███    ███   ██▄     █
# █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
# ██████████████████████████████████████████████████████████████████████████████
# █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
# ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
################################################################################
# Author: Patricio Reus Merlo
# Date: 05/16/2024
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
# PYTHON MODULES
################################################################################

import argparse
import os

################################################################################
# LOCAL MODULES
################################################################################

from tools import builder

################################################################################
# COMMAND LINE ARGUMENTS
################################################################################

parser = argparse.ArgumentParser(description="Test")

parser.add_argument("-f","--full",
                    action="store_true",
                    default=False, 
                    help="Compile core full version",)

parser.add_argument("-p", "--posedge",
                    action="store_true",
                    default=False, 
                    help="Compile core posedge version")

parser.add_argument("-d", "--delete",
                    action="store_true",
                    default=False,
                    help="Delete old compilation files")

args = parser.parse_args()

################################################################################
# RELATIVE PATHS AND COMMANDS
################################################################################

# Inc, src and CMake 
PRJ_DIR = "../"
SRC_DIR = "../src/"
CMAKE_FILE = "../CMakeLists.txt"

# CMake: core full version
CORE_FULL_DIR = "full_version/"
CORE_FULL_CMAKE_CMD = "cmake -DRUN_POSEDGE_LOGIC_ONLY=0 ../.. 2>&1 | tee cmake_log.txt"

# CMake: core posedge version
CORE_POSEDGE_DIR = "posedge_version/"
CORE_POSEDGE_CMAKE_CMD = "cmake -DRUN_POSEDGE_LOGIC_ONLY=1 ../.. 2>&1 | tee cmake_log.txt"

# Make
MAKE_CMD = "make -j8 2>&1 | tee make_log.txt"

# Release directory (git submodule)
RELEASE_DIR = "../release/"
RELEASE_SRC_DIR = RELEASE_DIR + "src"
RELEASE_INC_DIR = RELEASE_DIR + "inc" 

################################################################################
# CREATE VERSION HASH
################################################################################

version_hash = builder.version_hash(SRC_DIR)

################################################################################
# DELETE OLD COMPILATION FILES
################################################################################
    
if args.delete:
    if os.path.exists(CORE_FULL_DIR):
        os.system(f"rm -f -R {CORE_FULL_DIR}/*")

    if os.path.exists(CORE_POSEDGE_DIR):
        os.system(f"rm -f -R {CORE_POSEDGE_DIR}/*")

################################################################################
# CREATE AND OR CLEAN RELEASE DIRECTORY
################################################################################

builder.make_dir(RELEASE_SRC_DIR);
builder.make_dir(RELEASE_INC_DIR);

if args.full or args.posedge:
    builder.remove_dir_content(RELEASE_SRC_DIR)
    builder.remove_dir_content(RELEASE_INC_DIR)

################################################################################
# BUILD CORE (FULL VERSION)
################################################################################

if args.full:
    builder.run_cmake_and_make(CORE_FULL_DIR, CORE_FULL_CMAKE_CMD, MAKE_CMD)

################################################################################
# BUILD CORE (POSEDGE VERSION)
################################################################################

if args.posedge:
    builder.run_cmake_and_make(CORE_POSEDGE_DIR, CORE_POSEDGE_CMAKE_CMD, MAKE_CMD)

################################################################################
# COPY FILES TO RELEASE DIRECTORY
################################################################################

if args.full:
    builder.copy_dir_content(CORE_FULL_DIR, RELEASE_SRC_DIR, "*.a")

if args.posedge:
    builder.copy_dir_content(CORE_POSEDGE_DIR, RELEASE_SRC_DIR, "*.a")

if args.full or args.posedge:
    for f in os.listdir(SRC_DIR):
        builder.copy_dir_content(os.path.join(SRC_DIR, f), RELEASE_INC_DIR, "*.h*")
        
    builder.text_to_file(version_hash, RELEASE_DIR, "release.hash")