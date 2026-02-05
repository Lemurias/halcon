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

import argparse

################################################################################
# OPTIONS CLASS
################################################################################

class Options:

    @classmethod
    def args(self):

        self.__parser = argparse.ArgumentParser()
        
        #######################################
        # SIMULATOR OPTIONS (lower case)
        #######################################

        self.__parser.add_argument("-d", "--delete",
                                    action="store_true",
                                    default=False,
                                    help="delete old compilation files and logs")

        self.__parser.add_argument("-f", "--compile_f",
                                    action="store_true",
                                    default=False, 
                                    help="compile in FULL version")

        self.__parser.add_argument("-p", "--compile_p",
                                    action="store_true",
                                    default=False, 
                                    help="compile in POSEDGE version")

        self.__parser.add_argument("-l", "--enable_logs",
                                    action="store_true",
                                    default=False, 
                                    help="enable compilation logs")

        self.__parser.add_argument("-r","--run_sim",
                                    action="store_true",
                                    default=False, 
                                    help="run simulator")
        
        self.__parser.add_argument("-t","--run_tests",
                                    action="store_true",
                                    default=False,
                                    help="run simulator tests")
                
        #######################################
        # TEST OPTIONS (upper case)
        #######################################

        self.__parser.add_argument("-R","--run_cases",
                                    action="store_true",
                                    default=False, 
                                    help="run all cases")
        
        self.__parser.add_argument("-M","--run_missing",
                                    action="store_true",
                                    default=False, 
                                    help="run missing cases")
        
        self.__parser.add_argument("-A","--run_again",
                                    action="store_true",
                                    default=False, 
                                    help="run all again")

        self.__parser.add_argument("-K","--kill",
                                    action="store_true",
                                    default=False, 
                                    help="kill running cases")
        
        self.__parser.add_argument("-S","--summary",
                                    action="store_true",
                                    default=False, 
                                    help="test summary")
        
        self.__parser.add_argument("-F","--folders_list",
                                    action="store_true",
                                    default=False, 
                                    help="case folders list")
        
        self.__parser.add_argument("-L","--live",
                                    action="store_true",
                                    default=False, 
                                    help="live_summary")

        self.__parser.add_argument("-D","--delete_test",
                                    action="store_true",
                                    default=False, 
                                    help="detele test folder")
        
        self.__parser.add_argument("-P","--processing",
                                    action="store_true",
                                    default=False, 
                                    help="run processing scripts")
        
        self.__parser.add_argument("-T","--latex",
                                    action="store_true",
                                    default=False, 
                                    help="export latex")

        #######################################
        # TEST SUBCOMMANDS (RUNNER CONTROL)
        #######################################
        self.__subgroup = self.__parser.add_argument_group("Subcommands for runner mode")
        self.__subgroup.add_argument("--mode",
                                    action="store",
                                    choices=['local', 'background'],
                                    default='background', 
                                    help="Execution mode (local or background)")
        self.__subgroup.add_argument("--threads",
                                    action="store",
                                    type=int,
                                    default=None, 
                                    help="Number of threads")
        self.__subgroup.add_argument("--memory",
                                    action="store",
                                    type=int,
                                    default=None, 
                                    help="Memory to use [GB]")

        return self.__parser.parse_args()