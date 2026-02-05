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
import collections
import copy
import functools
import os
import yaml

################################################################################
# DECORATORS
################################################################################

def keep_directory(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        current_directory = os.getcwd()
        try:
            return func(*args, **kwargs)
        finally:
            os.chdir(current_directory)
    return wrapper

################################################################################
# SIMULATOR HANDLER CLASS
################################################################################

class SimulatorHandler:

    ############################################################################
    # COMMAND OPTIONS
    ############################################################################

    # Clock edges
    __POSITIVE = "p"
    __NEGATIVE = "n"
    __BOTH = "b"

    # File type
    __TEXT = "t"
    __BINARY = "b"

    # File name type
    __LONG = "l"
    __SHORT = "s"

    # Format
    __DEFAULT = "0"
    __DOUBLE = "d"
    __UINT = "i"
    __HEX = "h"

    ############################################################################
    # CONSTRUCTOR
    ############################################################################

    def __init__(self):

        # Instance name
        self.__name = None

        # Directories
        self.project_abs_dir = os.getcwd()
        self.build_dir = "build/"
        self.conf_dir = "conf/"
        self.run_dir = "run/"
        self.logs_dir = None
        self.src_dir = "src/"
        self.tests_dir = "tests/"
        self.modules_dir = None
        self.core_dir = "../release/"
        self.__optional_dirs = ["modules_dir", "logs_dir"]
        
        # Files
        self.cmakelists_file = "CMakeLists.txt"
        self.command_file = "conf/command.cmd"
        self.settings_file = "conf/settings.yaml"
        self.hierarchy_file = "conf/hierarchy.txt"
        self.main_cpp_file = "src/main.cpp"
        self.main_py_file = "main.py"
        self.binary_file = "run/simulator"
        self.__required_files = ["cmakelists_file", "main_cpp_file", "main_py_file"]

        # HALCON-Core
        # IMPORTANT: only for Core development and test
        self.__core_build_enable = False
        self.__core_build_dir = "../build/"
        self.__core_build_cmd = "python3 build.py {}"
        self.__core_build_options = "-{} "
        self.__core_build_full_opc = "f"
        self.__core_build_pose_opc = "p"
        self.__core_build_delete_opc = "d"

        # Make options
        self.__make_cmd = "make {}"
        self.__make_options = "-{}{}"
        self.__make_install_cmd = "make install"
        self.__make_clean_cmd = "make clean"

        self.__make_n_cores_field = "j"
        self.__make_n_cores_value = 8

        # CMake options
        self.__cmake_cmd = "cmake {} .."
        self.__cmake_options = "-D{}={} "
        
        self.__cmake_target_field = "TARGET_FILE"
        self.__cmake_target_value = None

        self.__cmake_exe_name_field = "EXE_NAME"
        self.__cmake_exe_name_value = None

        self.__cmake_external_dir_field = "EXTERNAL_DIR"
        self.__cmake_external_dir_value = None

        self.__cmake_external_list_field = "EXTERNAL_LIST"
        self.__cmake_external_list_value = ""

        self.__cmake_local_dir_field = "LOCAL_DIR"
        self.__cmake_local_dir_value = None

        self.__cmake_local_list_field = "LOCAL_LIST"
        self.__cmake_local_list_value = ""

        self.__cmake_type_field = "CORE_TYPE"
        self.__cmake_full_value = "FULL"
        self.__cmake_pose_value = "POSEDGE"

        self.__cmake_core_dir_field = "CORE_DIR"
        self.__cmake_core_dir_value = None

        self.__cmake_install_dir_field = "INSTALL_DIR"
        self.__cmake_install_dir_value = None

        # Executable options
        self.__exe_run_cmd = "./{} {}"
        self.__exe_options = "-{} {} "
        self.__exe_settings_field = "s"
        self.__exe_hierarchy_field = "x"
        self.__exe_commands_field = "c"
        self.__exe_logs_field = "l"
        self.__exe_export_field = "e"
        
        # HALCON: commands
        self.__halcon_log_cmd = 'LOG -s {signal} -c {clock} -e {edge} -b {begin} -p {step} -d {end} -t {file_t} -n {file_n} -f {format}'
        self.__halcon_flog_cmd = 'FLOG -s {signal} -t {file_t} -n {file_n} -f {format}'
        self.__halcon_setv_cmd = 'SET -v "{value}" -s {signal} -c {clock} -e {edge} -b {begin} -d {end}'
        self.__halcon_setf_cmd = 'SET -f "{file}" -s {signal} -c {clock} -e {edge} -b {begin} -d {end}'
        
        self.__halcon_valid_edge = ["p", "n", "b"]
        self.__halcon_valid_file_t = ["t", "b"]
        self.__halcon_valid_file_n = ["l", "s"]
        self.__halcon_valid_format = ["0", "d", "i", "h"]
        
        self.__halcon_log_and_set = "LS"
        self.__halcon_only_log = "L"
        self.__halcon_no_log_no_set = "X"

        # Objects
        self.settings = {}
        self.__hierarchy = {}
        self.__is_hierarchy_available = False

        # MACRO comments
        self.__settings_begin_macro = "# @ AUTO_SETTINGS_BEGIN"
        self.__settings_end_macro = "# @ AUTO_SETTINGS_END"
        self.__hierarchy_begin_macro = "# @ AUTO_HIERARCHY_BEGIN"
        self.__hierarchy_end_macro = "# @ AUTO_HIERARCHY_END"

        # Modules (dependences)
        self.__local_modules_list = []
        self.__external_modules_list = []

        # Tests
        self.__tests_cmd = "python3 {}"
        self.__tests_list = []

    ############################################################################
    # MACROS
    ############################################################################

    @property
    def POSITIVE(self):
        return self.__POSITIVE

    @property
    def NEGATIVE(self):
        return self.__NEGATIVE

    @property
    def BOTH(self):
        return self.__BOTH

    @property
    def TEXT(self):
        return self.__TEXT

    @property
    def BINARY(self):
        return self.__BINARY
    
    @property
    def LONG(self):
        return self.__LONG

    @property
    def SHORT(self):
        return self.__SHORT

    @property
    def DEFAULT(self):
        return self.__DEFAULT

    @property
    def DOUBLE(self):
        return self.__DOUBLE

    @property
    def UINT(self):
        return self.__UINT

    @property
    def HEX(self):
        return self.__HEX
        
    ############################################################################
    # PRIVATE METHODS
    ############################################################################

    def __load_instance_name(self):
        with open(self.__absolute_path(self.main_py_file), 'r') as file:
            for line in file:
                if "SimulatorHandler()" in line:
                    self.__name = line.split('=')[0].strip()
                    break

    def __attributes(self):
        names = dir(self)
        for name in names:
            attribute = getattr(self, name)
            yield attribute, name

    def __absolute_path(self, relative_to_project_dir):
        if relative_to_project_dir == "":
            raise ValueError("Use 'None' to indicate optional paths. Do not use an empty string.")
        if relative_to_project_dir == None:
            return
        return os.path.join(self.project_abs_dir, relative_to_project_dir)
    
    def __check_dir_existence_or_create(self, path):
        if os.path.exists(path) and os.path.isdir(path):
            return
        os.makedirs(path, exist_ok=True)

    def __change_paths_to_absolute(self):
        for attr_func, attr_name in self.__attributes():
            if isinstance(attr_func, str) and not attr_name.startswith("_") \
                and (attr_name.endswith("_dir") or attr_name.endswith("_file")):
                setattr(self, attr_name, self.__absolute_path(attr_func))

    def __check_dir_existence(self, path):
        if os.path.exists(path) and os.path.isdir(path):
            return
        raise FileNotFoundError(f"The directory '{path}' does not exist or is not a directory.")

    def __check_file_existence(self, path):
        if os.path.exists(path) and os.path.isfile(path):
            return
        raise FileNotFoundError(f"The file '{path}' does not exist or is not a file.")

    def __check_directories_and_files(self):
        for attr_func, attr_name in self.__attributes():
            if not attr_name.startswith("_"):
                
                # Files
                if attr_name.endswith("_file") and attr_name in self.__required_files:
                    self.__check_file_existence(attr_func)

                # Directories
                if attr_name.endswith("_dir"):
                    
                    if attr_name in self.__optional_dirs:
                        if attr_func != None:
                            self.__check_dir_existence_or_create(attr_func)
                            continue
                    else:
                        if attr_func != None:
                            self.__check_dir_existence_or_create(attr_func)
                            continue
                        else:
                            raise ValueError(f"The {attr_name} directory is not optional.")

    def __get_core_options(self, value):
        return self.__core_build_options.format(value)

    def __get_cmake_option(self, field, value):
        return self.__cmake_options.format(field, value)

    def __get_cmake_common_options(self):
        cmake_options = self.__get_cmake_option(self.__cmake_target_field, \
                                                 self.__cmake_target_value)
        cmake_options += self.__get_cmake_option(self.__cmake_exe_name_field, \
                                                 self.__cmake_exe_name_value)
        cmake_options += self.__get_cmake_option(self.__cmake_external_dir_field, \
                                                 self.__cmake_external_dir_value)
        cmake_options += self.__get_cmake_option(self.__cmake_local_dir_field, \
                                                 self.__cmake_local_dir_value)
        cmake_options += self.__get_cmake_option(self.__cmake_core_dir_field, \
                                                 self.__cmake_core_dir_value)
        cmake_options += self.__get_cmake_option(self.__cmake_install_dir_field, \
                                                 self.__cmake_install_dir_value)
        cmake_options += self.__get_cmake_option(self.__cmake_external_list_field, \
                                                 self.__cmake_external_list_value)
        cmake_options += self.__get_cmake_option(self.__cmake_local_list_field, \
                                                 self.__cmake_local_list_value)
        return cmake_options

    def __get_make_options(self, field, value):
        return self.__make_options.format(field, value)

    def __get_exe_options(self, field, value):
        return self.__exe_options.format(field, value)

    @keep_directory
    def __run_core_builder(self, flags=""):
        os.chdir(self.__absolute_path(self.__core_build_dir))
        os.system(self.__core_build_cmd.format(flags))

    @keep_directory
    def __run_cmake(self, flags=""):
        os.chdir(self.build_dir)
        os.system(self.__cmake_cmd.format(flags))

    @keep_directory
    def __run_make(self, flags=""):
        os.chdir(self.build_dir)
        os.system(self.__make_cmd.format(flags))
        os.system(self.__make_install_cmd)

    @keep_directory
    def __run_exe(self, flags=""):
        os.chdir(self.run_dir)
        exe_name = os.path.basename(self.binary_file)
        os.system(self.__exe_run_cmd.format(exe_name, flags))

    @keep_directory
    def __update_handler(self):
        self.__run_exe(self.__get_exe_options(self.__exe_export_field, ""))
        self.__load_hierarchy()
        self.__load_settings()

    def __write_to_command_file(self, cmd):
        with open(self.command_file, "a") as file:
            file.write("\n" + cmd)

    def __load_settings(self):
        try:
            with open(self.settings_file, "r") as file:
                self.settings = yaml.full_load(file)
        except FileNotFoundError:
            text = f"The settings file '{self.settings_file}' does not exist."
            raise FileNotFoundError(text)

    def __detect_macros(self, begin_macro, end_macro):
        begin_i = None
        end_i = None

        with open(self.main_py_file, 'r') as file:
            lines = file.readlines()
        
        for i, line in enumerate(lines):
            if line.strip() == begin_macro:
                begin_i = i
            elif line.strip() == end_macro:
                end_i = i

        if begin_i is None or end_i is None:
            print(f"The {begin_macro} and/or {end_macro} macros were not found in the file.")

        return lines, begin_i, end_i

    def __save_settings(self):
        try:
            with open(self.settings_file, 'w') as file:
                yaml.dump(self.settings, file, default_flow_style=False)
        except Exception as e:
            print(f"An error occurred while writing settings to '{self.settings_file}': {e}")

    def __load_hierarchy(self):
        try:
            with open(self.hierarchy_file, "r") as file:
                lines = file.readlines()
        except FileNotFoundError:
            raise ValueError(f"Could not open the file '{self.hierarchy_file}'")
        
        for line in lines:
            type, name = line.strip().split()
                        
            if type.startswith("*"):
                self.__hierarchy[name] = self.__halcon_log_and_set
            elif type.startswith("+"):
                self.__hierarchy[name] = self.__halcon_only_log
            else:
                self.__hierarchy[name] = self.__halcon_no_log_no_set
        
        self.__is_hierarchy_available = True

    def __extract_hierarchy(self, setting):
        parts = setting.split("']['")
        return "']['".join(parts[:-1]) + "']"

    def __settings_to_string(self):
        # Create lines
        lines = []
        def recurse(settings, base_key):
            if isinstance(settings, dict):
                for sub_key, sub_settings in settings.items():
                    recurse(sub_settings, f"{base_key}['{sub_key}']")
            else:
                lines.append(f"{base_key} = {repr(settings)}")
        
        recurse(self.settings, f"{self.__name}.settings")
        
        # Sort lines
        hierarchies = collections.defaultdict(list)
        
        # Group settings by the root key
        for line in lines:
            hierarchy = self.__extract_hierarchy(line)
            hierarchies[hierarchy].append(line)
        
        # Sort the root keys and their respective settings
        sorted_hierarchy_keys = sorted(hierarchies.keys())
        sorted_lines = []
        
        for hierarchy in sorted_hierarchy_keys:
            sorted_group = sorted(hierarchies[hierarchy])
            
            # Add a blank line between groups
            #sorted_group.append("")
            sorted_lines.extend(sorted_group)
        
        return "\n".join(sorted_lines).strip()
        
    def __check_if_it_is_loggeable(self, key):
        if self.__hierarchy[key] != self.__halcon_only_log \
            and self.__hierarchy[key] != self.__halcon_log_and_set:
            raise ValueError(f"The > {key} < signal does not support LOG commands.")

    def __check_if_it_is_setteable(self, key):
        if self.__hierarchy[key] != self.__halcon_log_and_set:
            raise ValueError(f"The > {key} < signal does not support SET commands.")

    def __check_if_edge_is_valid(self, key):
        if not key in self.__halcon_valid_edge:
            raise ValueError(f"The > {key} < edge is not valid.")

    def __check_if_linspace_is_valid(self, begin, step, end):
        if begin < 0 or end < 0:
            raise ValueError(f"BEGIN ({begin}) and END ({end}) keys must be positive.")

        if end != 0 and end < begin:
            raise ValueError(f"BEGIN ({begin}) key must be equal to or less than END ({end}) key.")
        
        if step < 1 or not isinstance(step, int):
            raise ValueError(f"STEP ({step}) key must be an integer and equal to or greater than 1.")

    def __check_if_file_type_is_valid(self, key):
        if not key in self.__halcon_valid_file_t:
            raise ValueError(f"The > {key} < file format is not valid.")

    def __check_if_file_name_type_is_valid(self, key):
        if not key in self.__halcon_valid_file_n:
            raise ValueError(f"The > {key} < file name format is not valid.")
        
    def __check_if_format_is_valid(self, key):
        if not key in self.__halcon_valid_format:
            raise ValueError(f"The > {key} < format is not valid.")
        
    def __get_modules(self, path):
        modules = []
        for element in os.listdir(path):
            if os.path.isdir(os.path.join(path, element)):
                modules.append(element)
        return modules

    ############################################################################
    # PUBLIC  METHODS
    ############################################################################

    def init(self):
        self.__change_paths_to_absolute()
        self.__check_directories_and_files()
        self.__load_instance_name()

        # CMake variables
        self.__cmake_target_value = self.main_cpp_file
        self.__cmake_exe_name_value = os.path.basename(self.binary_file)
        self.__cmake_external_dir_value = self.modules_dir
        self.__cmake_local_dir_value = self.src_dir
        self.__cmake_core_dir_value = self.core_dir
        self.__cmake_install_dir_value = self.run_dir

    def add_local_modules(self, modules):
        for module in modules:
            src_module_abs_dir = os.path.join(self.src_dir, module)
            self.__check_dir_existence(src_module_abs_dir)
            self.__local_modules_list.append(module)

        self.__cmake_local_list_value = '"{}"'.format(';'.join(self.__local_modules_list))

    def add_external_modules(self, modules):
        if self.modules_dir != None:
            for module in modules:            
                lib_module_abs_dir = os.path.join(self.modules_dir, module)
                self.__check_dir_existence(lib_module_abs_dir)
                self.__external_modules_list.append(module)
        else:
            raise ValueError("A valid directory for external modules was not specified.")
        
        self.__cmake_external_list_value = '"{}"'.format(';'.join(self.__external_modules_list))

    def add_tests(self, tests):
        for test in tests:
            self.__check_file_existence(os.path.join(self.tests_dir, test))

            if test.endswith(".py"):
                self.__tests_list.append(test)
            else:
                raise ValueError(f"The test {test} is not a Python file.")

    @keep_directory
    def delete_old_files(self):
        if self.__core_build_enable:
            self.__run_core_builder(self.__get_core_options(self.__core_build_delete_opc))
        os.chdir(self.build_dir)
        os.system(self.__make_clean_cmd)
        if os.path.exists("CMakeCache.txt"):
            os.remove("CMakeCache.txt")

    def enable_compilation_logs(self):
        self.__cmake_cmd = self.__cmake_cmd + " 2>&1 | tee cmake_log.txt"
        self.__make_cmd = self.__make_cmd + " 2>&1 | tee make_log.txt"
        self.__make_install_cmd = self.__make_install_cmd + " 2>&1 | tee make_install_log.txt"
        self.__core_build_cmd = self.__core_build_cmd + " 2>&1 | tee log.txt"

    def set_make_parallelism(self, n_cores=8):
        self.__make_n_cores_value = n_cores

    @keep_directory
    def compile_full(self):
        # Build Core
        if self.__core_build_enable:
            self.__run_core_builder(self.__get_core_options(self.__core_build_full_opc))
            
        # Run CMake and Make
        cmake_options = self.__get_cmake_common_options()
        cmake_options += self.__get_cmake_option(self.__cmake_type_field, \
                                                self.__cmake_full_value)
        self.__run_cmake(cmake_options)

        # Run Make
        make_options = self.__get_make_options(self.__make_n_cores_field, \
                                               self.__make_n_cores_value)
        self.__run_make(make_options)

        # Update handler
        self.__update_handler()
        
    @keep_directory
    def compile_posedge(self):
        # Build Core
        if self.__core_build_enable:
            self.__run_core_builder(self.__get_core_options(self.__core_build_pose_opc))

        # Run CMake
        cmake_options = self.__get_cmake_common_options()
        cmake_options += self.__get_cmake_option(self.__cmake_type_field, \
                                                self.__cmake_pose_value)
        self.__run_cmake(cmake_options)
        
        # Run Make
        make_options = self.__get_make_options(self.__make_n_cores_field, \
                                               self.__make_n_cores_value)
        self.__run_make(make_options)

        # Update handler
        self.__update_handler()

    def is_compiled(self):
        return os.path.exists(self.binary_file) and os.path.isfile(self.binary_file)

    def update_settings_section(self):
        self.__load_settings()
        lines, begin_i, end_i = self.__detect_macros(self.__settings_begin_macro, \
                                                     self.__settings_end_macro)
        # Remove old lines
        del lines[begin_i + 1 : end_i]

        # Write new lines
        settings_as_string = '\n' + self.__settings_to_string() + '\n\n'
        lines.insert(begin_i + 1, settings_as_string)

        # Update file
        with open(self.main_py_file, 'w') as file:
            file.writelines(lines)

    def save_settings_yaml(self):
        self.__save_settings()
        self.update_settings_section()

    def clean_command_file(self):
        with open(self.command_file, "w"):
            pass

    def check_if_exists_in_hierarchy(self, key):
        # Update hierarchy
        if not self.__is_hierarchy_available:
            self.__load_hierarchy()

        # Check key
        if not key in self.__hierarchy:
            raise ValueError(f"The '{key}' signal does not exist in the hierarchy.")

    def add_log(self, signals, clock, edge="p", begin=0, step=1, end=0, 
                       file_type="t", file_name_type="l", format="0"):
        
        # Support to list of signals or an individual signal
        if not isinstance(signals, list):
            signals = [signals]

        # Update hierarchy
        if not self.__is_hierarchy_available:
            self.__load_hierarchy()

        # Check log command fields
        self.check_if_exists_in_hierarchy(clock)
        self.__check_if_edge_is_valid(edge)
        self.__check_if_linspace_is_valid(begin, step, end)
        self.__check_if_file_type_is_valid(file_type)
        self.__check_if_file_name_type_is_valid(file_name_type)
        self.__check_if_format_is_valid(format)

        for signal in signals:

            # Check signal
            self.check_if_exists_in_hierarchy(signal)
            self.__check_if_it_is_loggeable(signal)

            # Build command
            command = self.__halcon_log_cmd.format(
                signal=signal,
                clock=clock,
                edge=edge,
                begin=begin,
                step=step,
                end=end,
                file_t=file_type,
                file_n=file_name_type,
                format=format
            )
            self.__write_to_command_file(command)

    def add_final_log(self, signal, file_type="t", file_name_type="l", format="0"):

        # Update hierarchy
        if not self.__is_hierarchy_available:
            self.__load_hierarchy()

        # Check log command fields
        self.check_if_exists_in_hierarchy(signal)
        self.__check_if_it_is_loggeable(signal)
        self.__check_if_file_type_is_valid(file_type)
        self.__check_if_file_name_type_is_valid(file_name_type)
        self.__check_if_format_is_valid(format)

        # Build command
        command = self.__halcon_flog_cmd.format(
            signal=signal,
            file_t=file_type,
            file_n=file_name_type,
            format=format
        )
        self.__write_to_command_file(command)

    def add_setv(self, value, signal, clock, edge="p", begin=0, end=0):

        # Check log command fields
        self.check_if_exists_in_hierarchy(signal)
        self.check_if_exists_in_hierarchy(clock) 
        self.__check_if_it_is_setteable(signal)
        self.__check_if_edge_is_valid(edge)
        self.__check_if_linspace_is_valid(begin, 1, end)

        # Build command
        command = self.__halcon_setv_cmd.format(
            value=value,
            signal=signal,
            clock=clock,
            edge=edge,
            begin=begin,
            end=end
        )
        self.__write_to_command_file(command)

    def add_setf(self, file, signal, clock, edge="p", begin=0, end=0):
        file_abs_path = os.path.join(self.project_abs_dir, file)

        # Check log command fields
        self.check_if_exists_in_hierarchy(signal)
        self.check_if_exists_in_hierarchy(clock)
        self.__check_if_it_is_setteable(signal)
        self.__check_if_edge_is_valid(edge)
        self.__check_if_linspace_is_valid(begin, 1, end)
        self.__check_file_existence(file_abs_path)

        # Build command    
        command = self.__halcon_setf_cmd.format(
            file=file_abs_path,
            signal=signal,
            clock=clock,
            edge=edge,
            begin=begin,
            end=end
        )
        self.__write_to_command_file(command)

    @keep_directory
    def get_available_external_modules(self):
        if self.modules_dir == None:
            return
        return self.__get_modules(self.modules_dir)
 
    @keep_directory
    def get_available_local_modules(self):
        return self.__get_modules(self.src_dir)

    @keep_directory
    def run(self):
        os.system(f"rm -rf {self.logs_dir}/*")
        exe_options = self.__get_exe_options(self.__exe_settings_field, self.settings_file)
        exe_options += self.__get_exe_options(self.__exe_hierarchy_field, self.hierarchy_file)
        exe_options += self.__get_exe_options(self.__exe_commands_field, self.command_file)
        exe_options += self.__get_exe_options(self.__exe_logs_field, self.logs_dir)
        self.__run_exe(exe_options)

    @keep_directory
    def run_tests(self):
        os.chdir(self.tests_dir)
        for test in self.__tests_list:
            os.system(self.__tests_cmd.format(test))

    def get_settings_copy(self):
        return copy.deepcopy(self.settings)
        
    def get_command_line_args(self):
        parser = argparse.ArgumentParser(description="Simulator Handler")
        
        parser.add_argument("-d", "--delete",
                            action="store_true",
                            default=False,
                            help="delete old compilation files and logs")

        parser.add_argument("-f", "--compile_f",
                            action="store_true",
                            default=False, 
                            help="compile in FULL version")

        parser.add_argument("-p", "--compile_p",
                            action="store_true",
                            default=False, 
                            help="compile in POSEDGE version")

        parser.add_argument("-l", "--enable_logs",
                            action="store_true",
                            default=False, 
                            help="enable compilation logs")

        parser.add_argument("-r","--run",
                            action="store_true",
                            default=False, 
                            help="run simulator")

        parser.add_argument("-t","--run_tests",
                            action="store_true",
                            default=False,
                            help="run tests")
        
        return parser.parse_args()