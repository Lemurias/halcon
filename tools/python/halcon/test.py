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

import collections
import os
import pickle
import sys
import tabulate
import time
import concurrent.futures

################################################################################
# HALCON MODULES
################################################################################

from .case import Case

################################################################################
# TEST CLASS
################################################################################

class Test:

    ############################################################################
    # CLASS MACROS
    ############################################################################

    PYTHON_DIR = "python/"
    PARAMETERS_FILE = "parameters.pkl"
    CASES_FILE = "cases.pkl"
    HYPERLINK_FMT = "\033]8;;file://{}\033\\{}\033]8;;\033\\"

    ############################################################################
    # CONSTRUCTOR
    ############################################################################

    def __init__(self, name, base_dir, parameters):
        
        # Check settings
        if not isinstance(name, str):
            raise TypeError("Attribute 'name' must be type str.")
        self.__name = '_'.join(word.lower() for word in name.split())

        # Base directory
        if not isinstance(base_dir, str):
            raise TypeError("Attribute 'base_dir' must be type str.")
        if not os.path.isabs(base_dir):
            base_dir = os.path.abspath(base_dir)
        self.__base_dir = base_dir
        
        # Test directory
        snake_case_name = '_'.join(word.lower() for word in self.__name.split())
        self.__test_dir = os.path.join(self.__base_dir, snake_case_name)
        
        # Parameter lists
        if not isinstance(parameters, list):
            raise TypeError("Attribute 'parameters' must be list of Parameter objects.")
        self.__parameters = parameters

        # Case lists
        self.__cases = []
        self.__case_names = []
        self.__case_descriptions = []
        self.__case_indices = []
        self.__case_directories = []

        # Python directory
        self.__python_dir = os.path.join(self.__test_dir, Test.PYTHON_DIR)
        
        # Files
        self.__summary_file = os.path.join(self.__test_dir, Case.SUMMARY_FILE)
        self.__cases_file = os.path.join(self.__python_dir, Test.CASES_FILE)
        self.__parameters_file = os.path.join(self.__python_dir, Test.PARAMETERS_FILE)

        self.__create_directories()

        # Runner Limiter
        self.__executor = concurrent.futures.ThreadPoolExecutor()
        self.__run_option = 'background'
        self.__memory_limit = None

    ############################################################################
    # PRIVATE METHODS
    ############################################################################

    def __create_directories(self):
        os.makedirs(self.__base_dir, exist_ok=True)
        os.makedirs(self.__test_dir, exist_ok=True)

    def __check_repeated_cases(self):
        counter = collections.defaultdict(int)
        for name in self.case_names:
            counter[name] += 1
        repeated = [name for name, count in counter.items() if count > 1]
        
        if len(repeated):
            text = f"there are {len(repeated)} repeated cases."
            raise NameError(text)

    def __run_cases(self, case_indices=None):
        if case_indices == None:
            case_indices = self.case_indices
            os.system(f"rm -f {self.__summary_file}")

        for case_i in case_indices:
            try:
                if self.__run_option == 'local':
                    self.__cases[case_i].run(self.__run_option)
                else:
                    self.__executor.submit(self.__cases[case_i].run)

            except KeyboardInterrupt:
                break
                
    def __filter_by_state(self, options):
        self.__load_summary()
        sc_zip = zip(self.case_statuses, self.case_indices)
        case_indices = [i for s, i in sc_zip if s in options]
        return case_indices

    def __load_summary(self):
        try:
            with open(self.__summary_file, "r") as file:
                lines = file.readlines()
        except FileNotFoundError:
            return
        
        for line in lines:
            case_i, case_pid, case_name, case_description = line.strip().split()
            case_i = int(case_i)
            if self.__cases[case_i].name == case_name:
                self.__cases[case_i].process_id = int(case_pid)

    ############################################################################
    # SETTERS AND GETTERS
    ############################################################################

    @property
    def directory(self):
        return self.__test_dir

    @property
    def case_names(self):
        if not self.__case_names or len(self.__case_names) < len(self.__cases):
            self.__case_names = [c.name for c in self.__cases]
        return self.__case_names

    @property
    def case_descriptions(self):
        if not self.__case_descriptions or len(self.__case_descriptions) != len(self.__cases):
            self.__case_descriptions = [c.description for c in self.__cases]
        return self.__case_descriptions

    @property
    def case_indices(self):
        if not self.__case_indices or len(self.__case_indices) != len(self.__cases):
            self.__case_indices = [c.index for c in self.__cases]
        return self.__case_indices
    
    @property
    def case_directories(self):
        if not self.__case_directories or len(self.__case_directories) != len(self.__cases):
            self.__case_directories = [c.base_directory for c in self.__cases]
        return self.__case_directories
    
    @property
    def case_hyperlinks(self):
        return [Test.HYPERLINK_FMT.format(c.base_directory, c.name) for c in self.__cases]

    @property
    def case_ids(self):
        return [c.process_id for c in self.__cases]
    
    @property
    def case_statuses(self):
        return [c.status for c in self.__cases]
        
    @property
    def case_times(self):
        return [c.run_time for c in self.__cases]
    
    ############################################################################
    # PUBLIC  METHODS
    ############################################################################

    def add(self, case):
        case.test_directory = self.__test_dir
        case.memory_limit = self.__memory_limit
        self.__cases.append(case)
        self.__case_names.append(case.name)
        self.__case_indices.append(case.index)
        self.__case_directories.append(case.base_directory)

    def delete(self):
        os.system(f"rm -rf {self.__test_dir}")

    def run(self):
        self.__check_repeated_cases()
        for case in self.__cases:
            case.delete()
            case.create_directory()
            case.copy_files()
        self.__run_cases()
        self.save_cases()

    def run_again(self):
        running_indices = self.__filter_by_state(Case.RUNNING)
        for case_i in running_indices:
            self.__cases[case_i].kill()
        for case in self.__cases:
            case.clean()
        self.__run_cases()
        self.save_cases()

    def run_missing(self):
        self.__check_repeated_cases()
        options = [Case.CREATED, Case.CLONED, Case.FAILED, Case.KILLED]
        missing_indices = self.__filter_by_state(options)
        for case_i in missing_indices:
            self.__cases[case_i].create_directory()
            self.__cases[case_i].copy_files()
            self.__cases[case_i].clean()
        self.__run_cases(missing_indices)
        self.save_cases()

    def save_cases(self):
        os.makedirs(self.__python_dir, exist_ok=True)
        
        with open(self.__parameters_file, 'wb') as file:
            pickle.dump(self.__parameters, file)
        
        with open(self.__cases_file, 'wb') as file:
            pickle.dump(self.__cases, file)
        
    def load_cases(self):
        with open(self.__parameters_file, 'rb') as file:
            self.__parameters = pickle.load(file)

        with open(self.__cases_file, 'rb') as file:
            cases = pickle.load(file)
        for case in cases:
            self.add(case)
        
    def summary(self):
        self.__load_summary()
        print (
            tabulate.tabulate (
                list(zip(
                        self.case_indices, 
                        self.case_ids, 
                        self.case_statuses, 
                        self.case_times,
                        self.case_hyperlinks,
                        self.case_descriptions
                    )), 
                headers = ["I", "ID", "STATUS", "TIME", "NAME", "DESCRIPTION"], 
                tablefmt = "rounded_outline",
                stralign = 'center',
            )
        )

        for state, count in collections.Counter(self.case_statuses).items():
            print(f" {state}: {count} ", end="")
        print(f" TOTAL: {len(self.case_statuses)}")

    def folders_list(self):
        self.__load_summary()
        print (
            tabulate.tabulate (
                list(zip(
                        self.case_indices, 
                        self.case_ids, 
                        self.case_statuses, 
                        self.case_directories
                    )), 
                headers = ["I", "ID", "STATUS", "DIRECTORY"],
                tablefmt = "rounded_outline",
                stralign = 'center',
            )
        )

        for state, count in collections.Counter(self.case_statuses).items():
            print(f" {state}: {count} ", end="")
        print(f" TOTAL: {Case.counter}")

    def live(self):
        while True:
            try:
                self.__load_summary()
                sys.stdout.write(Case.CLS_CMD)
                for state, count in collections.Counter(self.case_statuses).items():
                    sys.stdout.write(f" {state}: {count} ")
                sys.stdout.write(f" TOTAL: {Case.counter}")
                sys.stdout.flush()
                time.sleep(1)
            except KeyboardInterrupt:
                sys.stdout.write(Case.CLS_CMD)
                break

    def kill(self):
        running_indices = self.__filter_by_state(Case.RUNNING)
        for case_i in running_indices:
            self.__cases[case_i].kill()

    def set_config(self, run_option, n_threads, memory):
        self.__run_option = run_option
        self.__executor = concurrent.futures.ThreadPoolExecutor(max_workers=n_threads)
        self.__memory_limit = memory