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
# Date: 05/28/2024
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

import copy
import hashlib
import os
import pickle
import psutil
import subprocess
import sys
import yaml
import time

from colorama import Fore, Style

################################################################################
# HALCON MODULES
################################################################################

from .simulator_handler import SimulatorHandler

################################################################################
# CASE CLASS
################################################################################

class Case:

    ############################################################################
    # CLASS MACROS
    ############################################################################

    # Status
    CREATED = Fore.LIGHTMAGENTA_EX + "CREATED" + Style.RESET_ALL
    CLONED = Fore.LIGHTBLUE_EX + "CLONED" + Style.RESET_ALL
    RUNNING = Fore.LIGHTYELLOW_EX + "RUNNING" + Style.RESET_ALL
    FAILED = Fore.LIGHTRED_EX + "FAILED" + Style.RESET_ALL
    KILLED = Fore.LIGHTRED_EX + "KILLED" + Style.RESET_ALL
    FINISHED = Fore.LIGHTGREEN_EX + "FINISHED" + Style.RESET_ALL

    # Directories
    RUN_DIR = "run"
    CONF_DIR = "conf"
    LOGS_DIR = "logs"

    # Files
    BINARY_FILE = "binary"
    SETTINGS_FILE = "settings.yaml"
    COMMAND_FILE = "command.cmd"
    PID_FILE = "pid.txt"
    STDOUT_FILE = "stdout.txt"
    STDERR_FILE = "stderr.txt"
    TIME_FILE = "time.txt"
    SUMMARY_FILE = "summary.txt"

    # Commands
    RUN_CMD = "{} > {} 2> {} & echo $! | tee {}"
    RUN_LOCAL_CMD = "{} | tee {} 2> {} & echo $! | tee {}"
    PID_CMD = "ps aux | grep {} | grep -v grep | awk '{{print $2}}' | grep {}"
    CLS_CMD = "\r\033[K"

    # Name
    NAME_LENGTH = 16

    ############################################################################
    # CLASS ATRIBUTTES
    ############################################################################

    counter = 0

    ############################################################################
    # CONSTRUCTOR
    ############################################################################

    def __init__(self, simh):
        
        # Check simulator handler
        if not isinstance(simh, SimulatorHandler):
            raise TypeError("Attribute 'simh' must be type SimulatorHandler.")
        self.__simh = copy.deepcopy(simh)

        # Information
        self.__name = None
        self.__index = Case.counter
        self.__process_id = None
        self.__case_settings = {}
        self.__full_settings = {}
        
        # Directories and files
        self.__test_dir = None
        self.__base_dir = None
        self.__run_dir = None
        self.__conf_dir = None
        self.__logs_dir = None
        self.__binary_file = None
        self.__settings_file = None
        self.__command_file = None
        self.__pid_file = None
        self.__stdout_file = None
        self.__stderr_file = None
        self.__time_file = None
        self.__summary_file = None

        # Parameters
        self.__parameters = []

        # Update class counter
        Case.counter = Case.counter + 1

        # Resource Limiter
        self.memory_limit = None

    def __repr__(self):
        return f"\nCase(name={self.name}, index={self.index}, status={self.status})"

    ############################################################################
    # PRIVATE METHODS
    ############################################################################

    def __update_settings(self):
        for parameter in self.__parameters:
            keys = (parameter.name).split(".")
            current_dict = self.__case_settings
            for key in keys[:-1]:
                current_dict.setdefault(key, {})
                current_dict = current_dict[key]
            current_dict[keys[-1]] = parameter.value

    def __merge_settings(self, destination, source):
        for key, value in source.items():
            if isinstance(value, dict) and key in destination:
                self.__merge_settings(destination[key], value)
            else:
                destination[key] = value

    def __is_running(self):
        if self.process_id == None:
            return False
        try:
            process = psutil.Process(self.process_id)
        except psutil.NoSuchProcess:
            return False
        else:
            return process.is_running() and process.status() != psutil.STATUS_ZOMBIE

    def __generate_unique_key(self):
        parameters_str = (''.join(map(str, self.__parameters))).encode('utf-8')
        hash_obj = hashlib.sha256()
        hash_obj.update(parameters_str)
        return hash_obj.hexdigest()[: Case.NAME_LENGTH]
    
    def __to_hh_mm_ss(self, s):
        h = s // 3600
        m = (s % 3600) // 60
        s = s % 60
        return "{:02d}:{:02d}:{:02d}".format(int(h), int(m), int(s))
    
    def __resources_control(self):
        memory_usage = psutil.virtual_memory().used / (1024 * 1024)
        if self.memory_limit == None:
            return True
        else:
            return memory_usage < (self.memory_limit * 1024)

    ############################################################################
    # SETTERS AND GETTERS
    ############################################################################

    @property
    def name(self):
        if self.__name == None:
            self.__name = self.__generate_unique_key()
        return self.__name

    @property
    def description(self):
        return '_'.join([p.description for p in self.__parameters])

    @property
    def settings(self):
        self.__update_settings()
        return copy.deepcopy(self.__case_settings)

    @settings.setter
    def settings(self, settings):
        if not isinstance(settings, dict):
            raise TypeError("Attribute 'settings' must be type dict.")
        self.__case_settings = settings

    @property
    def test_directory(self):
        return self.__test_dir

    @test_directory.setter
    def test_directory(self, directory):
        self.__test_dir = directory
        self.__base_dir = os.path.join(self.__test_dir, self.name)
        self.__run_dir = os.path.join(self.__base_dir, Case.RUN_DIR)
        self.__conf_dir = os.path.join(self.__base_dir, Case.CONF_DIR)
        self.__logs_dir = os.path.join(self.__run_dir, Case.LOGS_DIR)
        self.__settings_file = os.path.join(self.__conf_dir, Case.SETTINGS_FILE)
        self.__command_file = os.path.join(self.__conf_dir, Case.COMMAND_FILE)
        self.__binary_file = os.path.join(self.__run_dir, Case.BINARY_FILE)
        self.__pid_file = os.path.join(self.__run_dir, Case.PID_FILE)
        self.__stdout_file = os.path.join(self.__run_dir, Case.STDOUT_FILE)
        self.__stderr_file = os.path.join(self.__run_dir, Case.STDERR_FILE)
        self.__time_file = os.path.join(self.__run_dir, Case.TIME_FILE)
        self.__summary_file = os.path.join(self.__test_dir, Case.SUMMARY_FILE)

    @property
    def base_directory(self):
        return self.__base_dir

    @property
    def run_directory(self):
        return self.__run_dir

    @property
    def conf_directory(self):
        return self.__conf_dir 

    @property
    def logs_directory(self):
        return self.__logs_dir

    @property
    def settings_file(self):
        return self.__settings_file

    @property
    def command_file(self):
        return self.__command_file

    @property
    def binary_file(self):
        return self.__binary_file

    @property
    def pid_file(self):
        return self.__pid_file

    @property
    def stdout_file(self):
        return self.__stdout_file

    @property
    def stderr_file(self):
        return self.__stderr_file

    @property
    def time_file(self):
        return self.__time_file

    @property
    def summary_file(self):
        return self.__summary_file

    @property
    def process_id(self):
        if self.__process_id == None:
            try:
                with open(self.__pid_file, 'r') as file:
                    self.__process_id = int(file.read().strip())
            except:
                self.__process_id = None
        return self.__process_id
    
    @process_id.setter
    def process_id(self, new_id):
        self.__process_id = new_id

    @property
    def status(self):
        if self.__is_running():
            return Case.RUNNING
        elif os.path.isfile(self.__stderr_file) and os.stat(self.__stderr_file).st_size:
            return Case.FAILED
        elif os.path.isfile(self.__stderr_file) and os.path.isfile(self.__time_file):
            return Case.FINISHED
        elif os.path.isfile(self.__stderr_file) and not os.path.isfile(self.__time_file):
            return Case.KILLED  
        elif os.path.isdir(self.__base_dir):
            return Case.CLONED
        else:
            return Case.CREATED

    @property
    def run_time(self):
        if os.path.isfile(self.__time_file):
            with open(self.__time_file) as file:
                run_time, loop_time = map(int, file.readline().strip().split(','))
            run_time = self.__to_hh_mm_ss(run_time * 1e-6)
        else:
            run_time = None
        return run_time
            
    @property
    def index(self):
        return self.__index

    ############################################################################
    # PUBLIC  METHODS
    ############################################################################

    def add(self, parameter):
        self.__parameters.append(parameter)

    def delete(self):
        os.system(f"rm -rf {self.__base_dir}")

    def create_directory(self):
        os.makedirs(self.__run_dir, exist_ok=True)
        os.makedirs(self.__conf_dir, exist_ok=True)

    def copy_files(self):

        sys.stdout.write(Case.CLS_CMD)
        sys.stdout.write(f"{self.__index} {Case.CLONED} {self.name} {self.description}")
        sys.stdout.flush()

        os.system(f"cp {self.__simh.settings_file} {self.__settings_file}")
        os.system(f"cp {self.__simh.command_file} {self.__command_file}")
        os.system(f"cp {self.__simh.binary_file} {self.__binary_file}")

        with open(self.__settings_file, "r+") as file:
            self.__full_settings = yaml.full_load(file)
            self.__merge_settings(self.__full_settings, self.settings)
            file.seek(0)
            file.truncate()
            yaml.dump(self.__full_settings, file, default_flow_style=False)

    def clean(self):
        os.system(f"find {self.__run_dir} -mindepth 1 -not -wholename '{self.__binary_file}' -delete")

    def run(self, option = 'background'):
        
        sys.stdout.write(Case.CLS_CMD + f"{self.__index} {Case.RUNNING} {self.name} {self.description}")
        sys.stdout.flush()

        if option == 'local':
            cmd_option = Case.RUN_LOCAL_CMD
        else:
            while not self.__resources_control():
                time.sleep(1)
            cmd_option = Case.RUN_CMD

        cmd = cmd_option.format (
                self.__binary_file,
                self.__stdout_file,
                self.__stderr_file, 
                self.__pid_file
        )
        
        process = subprocess.Popen (
            cmd,
            stdout = subprocess.PIPE, 
            stderr = subprocess.PIPE, 
            cwd = self.__run_dir,
            shell = True
        )

        self.__process_id = int(process.stdout.readline().strip())
        text = f"{self.__index} {self.__process_id} {self.name} {self.description}"
        os.system(f'echo "{text}" >> {self.__summary_file}')

        if option == 'local':
            process.wait()
            sys.stdout.write('\n')
            for line in process.stdout.readlines():
                sys.stdout.write(line.decode())
            for line in process.stderr.readlines():
                sys.stdout.write(line.decode())
            sys.stdout.flush()

    def kill(self):
        try:
            process = psutil.Process(self.process_id)
        except:
            return
        else:
            process.kill()

            sys.stdout.write(Case.CLS_CMD)
            sys.stdout.write(f"{self.__index} {Case.KILLED} {self.name} {self.description}")
            sys.stdout.flush()

    def get_setting(self, name):
        keys = name.split('.')
        value = self.settings
        try:
            for key in keys:
                value = value[key]
            return value
        except (KeyError, TypeError):
            raise KeyError(f"Key path '{name}' not found in settings dict")

    def get_parameter_value(self, name):
        parameters_dict = {p.name: p.value for p in self.__parameters}
        return parameters_dict[name]

    def get_parameter_index(self, name):
        parameters_dict = {p.name: p.index for p in self.__parameters}
        return parameters_dict[name]