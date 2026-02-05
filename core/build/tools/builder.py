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

import os

#######################################################################
#     Create a unique hash based on the size of the source files
#######################################################################

def version_hash(src_dir):
    
    if os.path.exists("checksum.txt"):
        os.remove("checksum.txt")

    for f in os.listdir(src_dir):
        os.system(f"cksum {os.path.join(src_dir, f)}/* >> checksum.txt")

    with os.popen("cat checksum.txt | sort -k 2 | cksum | cut -d' ' -f1") as p:
        version_hash = p.readline().strip();
    os.remove("checksum.txt")
    
    return version_hash

#######################################################################
#                       Make a new directory
#######################################################################

def make_dir(dir):
    os.makedirs(dir, exist_ok=True)

#######################################################################
#            Copy file from some directory to another one
#######################################################################

def copy_file(source_file, destination_file):
    os.system(f"cp {source_file} {destination_file}")

#######################################################################
#                       Remove only one file
#######################################################################

def remove_file(file):
    os.system(f"rm {file}")

#######################################################################
#       Copy all the content of some directory to another one
#######################################################################

def copy_dir_content(source_dir, destination_dir, filter=None):
    if not os.path.exists(source_dir):
        raise ValueError(f"[ERROR] {source_dir} doesn't exist ...")
    
    if not os.path.exists(destination_dir):
        raise ValueError(f"[ERROR] {destination_dir} doesn't exist ...")
    
    if filter == None:
        os.system(f"cp -f -r {source_dir}/* {destination_dir}")    
    else:
        os.system(f"cp -f -r {source_dir}/{filter} {destination_dir}")

#######################################################################
#       Move all the content of some directory to another one
#######################################################################

def move_dir_content(source_dir, destination_dir):
    if not os.path.exists(source_dir):
        raise ValueError(f"[ERROR] {source_dir} doesn't exist ...")
    
    if not os.path.exists(destination_dir):
        raise ValueError(f"[ERROR] {destination_dir} doesn't exist ...")
    
    os.system(f"mv -f {source_dir}/* {destination_dir}")

#######################################################################
#                           Clean directory
#######################################################################

def remove_dir_content(dir):
    os.system(f"rm -rf {dir}/*");

#######################################################################
#                           Remove directory
#######################################################################

def remove_dir(dir):
    os.system(f"rm -r {dir}");

#######################################################################
#                      Run CMake and the Make
#######################################################################

def run_cmake_and_make(dir, cmake_cmd, make_cmd):
    base_dir = os.getcwd();
    os.makedirs(dir, exist_ok=True)
    os.chdir(dir)
    os.system(cmake_cmd)
    os.system(make_cmd)
    os.chdir(base_dir)

#######################################################################
#                      Export text to file
#######################################################################
    
def text_to_file(text, dir, file_name):
    os.system(f"echo {text} > {dir}/{file_name}")