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
# Author:
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
import re
import random
import string

class Obfuscator:
    
    def __init__(self,
                 paths,
                 excepted_keys,
                 excepted_files_HARD,
                 excepted_files_SOFT,
                 file_header,
                 rng_seed = 0,
                 enable_headers_rename = True,
                 enable_comment_remover = True,
                 enable_spaces_remover = True) -> None:

        # Paths to files to be obfuscated
        self.paths = paths
        
        # Files that will not be modified at all
        self.excepted_files_HARD = excepted_files_HARD
        
        # Files that will be only modified with the keys corresponding to other files 
        self.excepted_files_SOFT = excepted_files_SOFT
        
        # Excepted keys
        self.excepted_keys = excepted_keys

        # Common file header (license and proj name)
        self.file_header = file_header

        # Enable headers renaming
        self.enable_headers_rename = enable_headers_rename

        # Enable comments removing
        self.enable_comment_remover = enable_comment_remover
        
        # Enable spaces, line breaks and indentation removing
        self.enable_spaces_remover = enable_spaces_remover
        
        # C++20 keywords    
        self.cpp_keywords = [
            'alignas', 'alignof', 'and', 'and_eq', 'array', 'asm', 'auto', 'bitand', 'bitor',
            'bool', 'break', 'case', 'catch', 'char', 'char8_t', 'char16_t', 'char32_t',
            'class', 'co_await', 'co_return', 'co_yield', 'compl', 'concept', 'const',
            'consteval', 'constexpr', 'const_cast', 'continue', 'decltype', 'default',
            'delete', 'do', 'double', 'dynamic_cast', 'else', 'enum', 'explicit',
            'export', 'extern', 'false', 'float', 'for', 'friend', 'goto', 'if',
            'inline', 'int', 'long', 'map', 'mutable', 'namespace', 'new', 'noexcept', 'not',
            'not_eq', 'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected',
            'public', 'register', 'reinterpret_cast', 'requires', 'return', 'short',
            'signed', 'size_t', 'sizeof', 'static', 'static_assert', 'static_cast', 'string',
            'std', 'struct', 'switch', 'template', 'this', 'thread_local', 'throw',
            'true', 'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned', 'using',
            'vector', 'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq',
            # ofstream
            'out', 'app',
            # map
            'count',
        ]

        # C++ preprocessor keywords
        self.cpp_preprocessor_keywords = [
            '#define', '#undef', '#ifdef', '#ifndef', '#if', '#else', '#elif', '#endif',
            '#include', '#ifdef', '#ifndef', '#else', '#define', '#undef', '#pragma',
            '#error', '#line', '#region', '#endregion', 'once'
        ]

        self.hash_name_dict = dict()

        self.rng_seed = rng_seed

    def strings_to_hex(self, original_string):
        """
        Converts string chars to hexa representation
        """
        split_code = re.split("\n", original_string)
        new_string = ""
        index = 0
        for line in split_code:
                
            # Skips header include lines
            if not "#include" in line:

                # Encoding strings to bytes
                pattern = re.compile(r'"([^"]*)"')
                matches = pattern.findall(line)

                def get_ascii_bytes(char_value):
                    dec_val = ord(char_value)
                    hex_val = hex(dec_val)
                    hex_val = hex_val[1:]
                    return f"\{hex_val}"

                # Convert every char in string to hexa
                for match in matches:
                    if match: # Skips empty strings ""
                        modified_string = ''.join(map(get_ascii_bytes, match))
                        line = line.replace(f'"{match}"', f'"{modified_string}"')
                
                # Restore '\n' and '\t' characters
                line = line.replace("\\x5c\\x6e", "\\n")
                line = line.replace("\\x5c\\x74", "\\t")

            if(index >= 1):
                new_string = new_string + "\n" + line  
            else:
                new_string = new_string + line
            
            index+=1

        return new_string
    
    def read_keys(self, original_string):
        """
        Identifies all variables and function names and assings a random string to each one
        """
        original_string = self.strings_to_hex(original_string)

        # Regular expression to find all function and variable names
        filtered_code = re.findall("(?:\w+\d*\s+)(?:\**)([a-zA-Z_][a-zA-Z0-9_]*)",
                                   self.comment_remover(original_string))
            
        # Adding function and variable names to dictionary
        for found_key in filtered_code:
            
            if  (found_key not in self.cpp_keywords) and \
                (found_key not in self.cpp_preprocessor_keywords) and \
                (found_key not in self.excepted_keys):
                
                if(hash(found_key) not in self.hash_name_dict):
                    
                    self.hash_name_dict[hash(found_key)] = (found_key, self._random_string())
    
    def renamer(self, original_string):
        """
        Renames all variables and fuctions
        """
        
        split_code = re.split("\n", original_string)
        
        new_string = ""
        index = 0
        for line in split_code:
                
                # Skips header include lines
                if not "#include" in line:

                    line = self.strings_to_hex(line)
                    
                    # Replacing keys
                    for key_hash in self.hash_name_dict:
                        
                        entry = self.hash_name_dict[key_hash][0]

                        # Used \W because we dont want to replace a variable if it is inside another word.                 
                        re_string = r"\W{}\W|^{}\W|\W{}$".format(entry,entry,entry)

                        # While loop to go through every entry and replace it
                        while True:
                            
                            first_found_entry = re.search(re_string, line)

                            # Breaks when it cannot find another instance
                            if not first_found_entry:
                                break

                            # Gets the iterator start and end points of the searched re_string
                            start = first_found_entry.start(0)
                            end = first_found_entry.end(0)

                            for i in range(0, len(first_found_entry.group(0))):
                                if line[start+i] == entry[0]:
                                    start = start + i
                                    end = start + len(entry)
                                    break
                            
                            # Replacing the var/func name with the random string
                            line = line[:start] + self.hash_name_dict[key_hash][1] + line[end:]
                
                if(index >= 1):
                    new_string = new_string + "\n" + line  
                else:
                    new_string = new_string + line
                
                index+=1
        
        # Return the obfuscated code
        return new_string

    def _random_string(self, stringLength=None):
        """
        Generates a random string
        """
        if stringLength is None:
            stringLength = random.randint(7, 21)
        letters = string.ascii_lowercase
        return ''.join(random.choice(letters) for i in range(stringLength))
    
    def whitespaces_remover(self, original_string):
        """
        Removes all whitespaces
        """

        split_code = re.split("\n", original_string)
        
        # Removing spaces for non-preprocessor lines
        new_string = ""
        for line in split_code:
            words = line.split()
            preprocessor_line = False
            for word in words:
                if word in self.cpp_preprocessor_keywords:
                    preprocessor_line = True
                    new_string = new_string + "\n" + line + "\n"
                    

            if not preprocessor_line:
                # Removing line break
                new_line = line.replace("\n", "")
                new_line = new_line.replace("\t", " ")
                new_line = new_line.replace("    ", " ")
                new_string = new_string + new_line

        return new_string
    
    def comment_remover(self, original_string):
        """
        Removes C++ style comments (single line and block comments)
        """
        
        # Single line comments
        new_string = ""
        split_code = re.split("\n", original_string)
        for line in split_code:
            idx = line.find("//")
            if idx != -1:
                new_line = line[:idx]
                new_string = new_string + new_line + "\n"
            else:
                new_string = new_string + line + "\n"

        # Block comments
        pattern = re.compile(r'/\*(.*?)\*/', re.DOTALL)
        new_string = pattern.sub('', new_string)
        
        return new_string
    
    def restore_file_header(self, original_string, header):
        """
        Restores file header in those files which it might been modified with obfuscator
        """
        # Block comments
        pattern = re.compile(r'/\*(.*?)\*/', re.DOTALL)

        modified_text = pattern.sub(header, original_string, count=1)

        return modified_text
    
    def add_file_header(self, original_string, header):
        """
        Adds file header in those files where comments have been removed
        """
        return header + "\n" + original_string
    
    def run(self):
        """
        Run obfuscator and generate new files
        """
        #######################################################################
        #                          NAME GENERATOR SEED
        #######################################################################
        
        random.seed(self.rng_seed)

        #######################################################################
        #                           HEADERS RENAMING
        #######################################################################
        if self.enable_headers_rename:

            # Random names for obfuscated headers
            filenames_dict = {}
            for path in self.paths:
                
                for filename in os.listdir(path):
                
                    if (filename.endswith(".hpp")) \
                        and (filename not in self.excepted_files_SOFT) \
                            and (filename not in self.excepted_files_HARD):
                        
                        filenames_dict[filename] = self._random_string()
                        os.rename(f"{path}/{filename}", f"{path}/{filenames_dict[filename]}.hpp")

            # Replacing random names in #include statements
            for path in self.paths:

                for filename in os.listdir(path):
                    
                    if (filename not in self.excepted_files_HARD) \
                        and (filename.endswith(".hpp") \
                            or (filename.endswith(".cpp")) \
                                or filename=="CMakeLists.txt"):
                        
                        with open(os.path.join(path, filename)) as file_data:

                            file_string = file_data.read()

                            for key in filenames_dict:
                                if filename=="CMakeLists.txt":
                                    file_string = file_string.replace(key, filenames_dict[key] + ".hpp")
                                else:
                                    file_string = file_string.replace(f'"{key}"', f'"{filenames_dict[key] + ".hpp"}"')

                        with open(os.path.join(path, filename), 'w') as file_data:

                            file_data.write(file_string)

        #######################################################################
        #                           KEYS RECOGNITION
        #######################################################################
        for path in self.paths:
            
            print(f"-- Loading files from {path}")
            for filename in os.listdir(path):
                
                if filename.endswith(".cpp") or filename.endswith(".hpp"):

                    if filename not in self.excepted_files_HARD:
                        
                        with open(os.path.join(path, filename)) as file_data:

                            file_string = file_data.read()
                            self.read_keys(file_string)

        #######################################################################
        #            KEYS REPLACEMENT SPACES AND COMMENTS REMOVING
        #######################################################################
        for path in self.paths:

            for filename in os.listdir(path):
                
                if filename.endswith(".cpp") or filename.endswith(".hpp"):
                    
                    print("-- File " + filename + " ", end="")
                    
                    if filename not in self.excepted_files_HARD:
                        
                        with open(os.path.join(path, filename)) as file_data:
                            
                            file_string = file_data.read()
                            
                            # COMMENT REMOVING
                            if (filename not in self.excepted_files_SOFT) and self.enable_comment_remover:
                                file_string = self.comment_remover(file_string)
                            
                            # KEYS RENAMING
                            file_string = self.renamer(file_string)
                            
                            # WHITESPACES REMOVING
                            if (filename not in self.excepted_files_SOFT) and self.enable_spaces_remover:
                                file_string = self.whitespaces_remover(file_string)
                            
                        with open(os.path.join(path, filename), 'w') as file_data:
                            
                            file_data.write(file_string)

                        print("is an OBFUSCATED version")

                    else:
                        print("is an ORIGINAL version")

def main():
    
    paths = ["../", "../src/", "../include/"]

    excepted_keys = ["T", "N", "type", "it", "set", "logger", "setter", "spdlog",
                    # CLI
                    "CLI", "App",
                    # YAMLCPP
                    'YAML', 'begin', 'end', 'size',
                    # Module
                    "Module", "Initialize", "settings_path", "settings", "GetBigMap",
                    "CreateSettingsTemplate", "CreateSettingsYAMLNodeRecursively",
                    "PrintBigMap", "Init", "Connect",
                    "RunClockMaster", "RunClockSlavePositiveEdge", "RunClockSlaveNegativeEdge",
                    "clk_id", "IS_COMBINATIONAL",
                    # Reflective
                    "REFLECT", "REFLECT_CUSTOM", "REFLECT_ARRAY", "REFLECT_ARRAY_OF_REGISTERS", "Reflect", "name",
                    # Simulator
                    "RUN_POSEDGE_LOGIC_ONLY", "_RUN_POSEDGE_LOGIC_ONLY",
                    "Simulator", "StartUp", "Connect", "Iteration", "ContinueRunning",
                    "Terminate", "ParseMainOptions", "argc", "argv", "Run",
                    "CoreInit", "SetCmdPath", "SetCfgPath", "GetFastestClockName",
                    "GetFastestClockFreq", "GetIterationCounter", "GetIterationRate",
                    "CreateSettingsTemplate", "cmd_path", "cfg_path", "settings_path",
                    "signal_log_path", "logger_max_buffer_size", "iteration_counter",
                    "fastest_clock", "clk_cmd_handler", "command_handler",
                    "tic_toc", "scheduler",
                    # TicToc
                    "Rate",
                    # Port
                    "Port", "ports", "p_data", "p_node", "p_module", "GetData", "GetModulePointer",
                    "PointTo", "data", "another", "pointer", "SetModulePtr",
                    "LoadDataTo", "IsNull", "Optimize", "Input", "Output",
                    # NodeVector
                    "NodeVector", "InputV", "OutputV",
                    # Register
                    "Register", "i", "o", "RunClock", "Reset", "Set",
                    # Clock
                    "Clock", "clock", "frequency", "period", "initial_phase", "state",
                    "last_edge_time", "time", "n_ticks", "GetId", "GetNextEdgeTime",
                    "GetLastEdgeTime", "GetNextEdgeType", "GetTickCount", "GetTime",
                    "GetFrequency", "GetState", "Init", "Configure", "settings",
                    "RunClockMaster", "RunClockSlave", "RegisterAsPosEdgeSensitive",
                    "RegisterAsNegEdgeSensitive", "module_ptr", "seed", "jitter_std",
                    "enable_colored_jitter", "path_to_jitter_mask", "enable_sin_jitter",
                    "sin_jitter_amplitude", "sin_jitter_frequency", "EdgeType", "State", "edge",
                    ];
    
    excepted_files_HARD = ["CLI11.hpp", "ac_fixed.h", "ac_int.h",]

    excepted_files_SOFT = [
                            "module.hpp", "module.cpp",
                            "simulator.hpp", "simulator.cpp", 
                            "port.hpp", "node_vector.hpp",
                            "register.hpp",
                            "clock.hpp", "clock.cpp",
                            "halcon.hpp", "tictoc.hpp"
                           ]
    
    file_header = """ ... """

    obf = Obfuscator(paths,
                     excepted_keys,
                     excepted_files_HARD,
                     excepted_files_SOFT,
                     file_header,
                     enable_headers_rename = True,
                     enable_comment_remover = True,
                     enable_spaces_remover = True)


    obf.run()

    
if __name__ == "__main__":
    main()
