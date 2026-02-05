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
# Date: 06/04/2024
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

import fnmatch
import glob
import numpy as np
import os
import pickle
import re
import tabulate

################################################################################
# HALCON MODULES
################################################################################

from .test import Test

################################################################################
# PROCESSOR CLASS
################################################################################

class Processor:
    
    ############################################################################
    # CLASS MACROS
    ############################################################################

    NUMPY_TYPE_MAP = {
        'bool': {'1': np.bool_},
        'char': {'1': np.byte},
        'unsigned_char': {'1': np.ubyte},
        'short': {'2': np.int16},
        'unsigned_short': {'2': np.uint16},
        'int': {'4': np.int32},
        'unsigned_int': {'4': np.uint32},
        'long': {'4': np.int32, '8': np.int64},
        'unsigned_long': {'4': np.uint32, '8': np.uint64},
        'long_long': {'8': np.int64},
        'unsigned_long_long': {'8': np.uint64},
        'float': {'4': np.float32},
        'double': {'8': np.float64},
        'long_double': {'16': np.longdouble},
        'cpx.float': {'8': np.complex64},
        'cpx.double': {'16': np.complex128},
        'cpx.long_double': {'32': np.clongdouble},
    }

    LOG_KEY = 'LOG'
    FLOG_KEY = 'FLOG'

    SIGNAL_KEY = '_SIGNAL_'
    CLOCK_KEY = '_CLOCK_'
    BEGIN_KEY = '_BEGIN_'
    END_KEY = '_END_'
    TYPE_KEY = '_TYPE_'
    BYTES_KEY = '_BYTES_'
    SIZE_KEY = '_SIZE_'
    
    LOG_SIGNAL_PATTERN = re.compile(SIGNAL_KEY + r'(.*?)' + CLOCK_KEY, re.DOTALL)
    LOG_CLOCK_PATTERN = re.compile(CLOCK_KEY + r'(.*?)' + BEGIN_KEY, re.DOTALL)
    LOG_BEGIN_PATTERN = re.compile(BEGIN_KEY + r'(.*?)' + END_KEY, re.DOTALL)
    LOG_END_PATTERN = re.compile(END_KEY + r'(.*?)' + TYPE_KEY, re.DOTALL)
    LOG_TYPE_PATTERN = re.compile(TYPE_KEY + r'(.*?)' + BYTES_KEY, re.DOTALL)
    LOG_BYTES_PATTERN = re.compile(BYTES_KEY + r'(.*?)' + SIZE_KEY, re.DOTALL)
    LOG_SIZE_PATTERN = re.compile(SIZE_KEY + r"(\d+)", re.DOTALL)
    
    FLOG_SIGNAL_PATTERN = re.compile(SIGNAL_KEY + r'(.*?)' + TYPE_KEY, re.DOTALL)
    FLOG_TYPE_PATTERN = re.compile(TYPE_KEY + r'(.*?)' + BYTES_KEY, re.DOTALL)
    FLOG_BYTES_PATTERN = re.compile(BYTES_KEY + r'(.*?)' + SIZE_KEY, re.DOTALL)
    FLOG_SIZE_PATTERN = re.compile(SIZE_KEY + r"(\d+)", re.DOTALL)

    ############################################################################
    # CONSTRUCTOR
    ############################################################################

    def __init__(self, test_dir):

        if not isinstance(test_dir, str):
            raise TypeError("Attribute 'test_dir' must be type str.")
        if not os.path.isdir(test_dir):
            raise ValueError("Attribute 'test_dir' must be a previously created directory.")
        if not os.path.isabs(test_dir):
            test_dir = os.path.abspath(test_dir)
        self.__test_dir = test_dir
        
        # Load files
        self.__python_dir = os.path.join(self.__test_dir, Test.PYTHON_DIR)
        self.__cases_file = os.path.join(self.__python_dir, Test.CASES_FILE)
        self.__parameters_file = os.path.join(self.__python_dir, Test.PARAMETERS_FILE)
        
        with open(self.__cases_file, 'rb') as file:
            self.__cases = pickle.load(file)
        
        self.__case_names = [c.name for c in self.__cases]
        self.__case_descriptions = [c.description for c in self.__cases]
        self.__case_indices = [c.index for c in self.__cases]
        self.__case_directories = [c.base_directory for c in self.__cases]

        with open(self.__parameters_file, 'rb') as file:
            self.__parameters = pickle.load(file)

    ############################################################################
    # CLASS METHODS
    ############################################################################

    @classmethod
    def load_directory(cls, directory):
        directory = os.path.abspath(directory)
        files = os.listdir(directory)
        files = [os.path.join(directory, file) for file in files]
        return Processor.load_files(files)

    @classmethod
    def load_files(cls, files):
        files = sorted(files)
        results = {}
        
        for file in files:
            
            # File information
            file_name, file_type = os.path.splitext(os.path.basename(file))
            
            if file_name.startswith(Processor.LOG_KEY):
                signal = Processor.LOG_SIGNAL_PATTERN.search(file_name).group(1)
                clock = Processor.LOG_CLOCK_PATTERN.search(file_name).group(1)
                begin = Processor.LOG_BEGIN_PATTERN.search(file_name).group(1)
                end = Processor.LOG_END_PATTERN.search(file_name).group(1)
                type = Processor.LOG_TYPE_PATTERN.search(file_name).group(1)
                bytes = Processor.LOG_BYTES_PATTERN.search(file_name).group(1)
                size = Processor.LOG_SIZE_PATTERN.search(file_name).group(1)
                
            elif file_name.startswith(Processor.FLOG_KEY):
                signal = Processor.FLOG_SIGNAL_PATTERN.search(file_name).group(1)
                type = Processor.FLOG_TYPE_PATTERN.search(file_name).group(1)
                bytes = Processor.FLOG_BYTES_PATTERN.search(file_name).group(1)
                size = Processor.FLOG_SIZE_PATTERN.search(file_name).group(1)
            
            else:
                continue
            
            # Data type
            if type in Processor.NUMPY_TYPE_MAP \
                and bytes in Processor.NUMPY_TYPE_MAP[type]:
                
                np_type = Processor.NUMPY_TYPE_MAP[type][bytes]
                np_bytes = np.dtype(np_type).itemsize
            else:
                raise TypeError(f"{type} type of {bytes} bytes is not supported.")

            if np_bytes != int(bytes):
                raise TypeError(f"NumPy data type has {np_bytes} bytes per element, "
                                f"but the file contains {bytes} bytes per element.")
            # Load data
            if file_type == ".txt":
                samples = np.loadtxt(file, dtype=np_type)

            elif file_type == ".bin":
                with open(file, 'rb') as bin_file:
                    samples = np.frombuffer(bin_file.read(), dtype=np_type)

                # Matrix conversion
                if size != "1":
                    samples = samples.reshape(-1, int(size))
            else:
                raise TypeError(f"Unknown file extension of {file}")

            # Save data
            data = dict()
            data['s'] = samples
            data['f'] = file_type

            if file_name.startswith(Processor.LOG_KEY):
                data['c'] = clock
                data['b'] = begin
                data['e'] = end
            
            if signal in results.keys():
                if not isinstance(results[signal], np.ndarray):
                    results[signal] = np.array(results[signal], dtype=object)
                results[signal] = np.append(results[signal], data)
            else:
                results[signal] = data
        return results

    ############################################################################
    # SETTERS AND GETTERS
    ############################################################################

    @property
    def cases(self):
        return self.__cases

    @property
    def directories(self):
        return self.__test_dir

    @property
    def names(self):
        return self.__case_names

    @property
    def descriptions(self):
        return self.__case_descriptions

    @property
    def indices(self):
        return self.__case_indices
    
    @property
    def directories(self):
        return self.__case_directories
    
    @property
    def ids(self):
        return [c.process_id for c in self.__cases]
    
    @property
    def statuses(self):
        return [c.status for c in self.__cases]
        
    @property
    def times(self):
        return [c.run_time for c in self.__cases]
    
    ############################################################################
    # PUBLIC  METHODS
    ############################################################################

    def print(self, cases=None):
        if cases == None:
            cases = self.__cases
        data = [(c.index, c.status, c.name, c.description) for c in cases]
        print (
            tabulate.tabulate (
                list(data), 
                headers = ["I", "STATUS", "NAME", "DESCRIPTION"], 
                tablefmt = "rounded_outline",
                stralign = 'center'
            )
        )

    def filter_by_name(self, patterns, cases=None):
        if cases == None:
            cases = self.__cases
        if not isinstance(patterns, list):
            patterns = [patterns]
        fcases = []
        for pattern in patterns:
            fcases.extend([case for case in cases if fnmatch.fnmatchcase(case.name, pattern)])
        return fcases

    def filter_by_description(self, patterns, cases=None):
        if cases == None:
            cases = self.__cases
        if not isinstance(patterns, list):
            patterns = [patterns]
        fcases = []
        for pattern in patterns:
            fcases.extend([case for case in cases if fnmatch.fnmatchcase(case.description, pattern)])
        return fcases

    def filter_by_status(self, patterns, cases=None):
        if cases == None:
            cases = self.__cases
        if not isinstance(patterns, list):
            patterns = [patterns]
        fcases = []
        for pattern in patterns:
            fcases.extend([case for case in cases if fnmatch.fnmatchcase(case.status, pattern)])
        return fcases

    def axis(self, param_name):
        for p in self.__parameters:
            if param_name == p.name:
                return p.value
        raise ValueError(f"Unknown parameter: {param_name}")
    
    def signals(self, patterns="*"):
        if not isinstance(patterns, list):
            patterns = [patterns]
        
        n_cases = len(self.__cases)
        n_params = len(self.__parameters)

        indices = np.empty((n_cases, 0), dtype=int)
        shape = np.empty(0, dtype=int)

        for p in self.__parameters:
            aux = np.array([[case.get_parameter_index(p.name)] for case in self.__cases])
            shape = np.append(shape, np.size(np.unique(aux)))
            indices = np.hstack([indices, aux])
        
        results = np.empty(shape, dtype=object)
        for case_i, case in enumerate(self.__cases):
            files = []
            for pattern in patterns:    
                files.extend(glob.glob(os.path.join(case.logs_directory, pattern)))
            index = tuple([indices[case_i, i] for i in range(n_params)])
            results[index] = self.load_files(files)
        return results