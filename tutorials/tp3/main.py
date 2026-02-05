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
# Author: Manuel Exequiel Arias
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

import os

################################################################################
# PyHAWK MODULES
################################################################################

from halcon import SimulatorHandler

################################################################################
# SIMULATOR HANDLER
################################################################################

simh = SimulatorHandler()

################################################################################
# COMMAND LINE ARGUMENTS
################################################################################

args = simh.get_command_line_args()

################################################################################
# SETTINGS: DIRECTORIES
################################################################################

simh.project_abs_dir = os.getcwd()
simh.build_dir = "build/"
simh.conf_dir = "conf/"
simh.run_dir = "run/"
simh.logs_dir = "run/logs/"
simh.src_dir = "src/"
simh.tests_dir = "tests/"
simh.modules_dir = None
simh.core_dir = "../lib/core/"

################################################################################
# SETTINGS: FILES
################################################################################

simh.cmakelists_file = "CMakeLists.txt"
simh.command_file = "conf/command.cmd"
simh.settings_file = "conf/settings.yaml"
simh.hierarchy_file = "conf/hierarchy.txt"
simh.main_cpp_file = "src/main.cpp"
simh.main_py_file = "main.py"
simh.binary_file = "run/debug_sim_tp3"

################################################################################
# SETTINGS: INIT
################################################################################

simh.init()

################################################################################
# SETTINGS: MODULES
################################################################################

if args.compile_f or args.compile_p:
    
    local_modules = simh.get_available_local_modules()
    simh.add_local_modules(local_modules)


################################################################################
# SETTINGS: TESTS
################################################################################

if args.run_tests:
    tests = ["plot.py"]
    simh.add_tests(tests)

################################################################################
# COMPILATION: DELETE OLD FILES
################################################################################

if args.delete:
    simh.delete_old_files()

################################################################################
# COMPILATION: ENABLE COMPILATION LOG
################################################################################

if args.enable_logs:
    simh.enable_compilation_logs()

################################################################################
# COMPILATION: MAKE PARALLELISM
################################################################################

if args.compile_f or args.compile_p:
    simh.set_make_parallelism(12)


################################################################################
# COMPILATION: CORE FULL
################################################################################

if args.compile_f:
    simh.compile_full()

################################################################################
# COMPILATION: CORE POSEDGE
################################################################################

if args.compile_p:
    simh.compile_posedge()

################################################################################
# SETTINGS: LOG COMMANDS
################################################################################

if args.run:
    simh.clean_command_file()
    
    # # Clock 
    simh.add_log("root.clk", "root.clk", "p", 0, 1, 0, "t")
    
    # # Filter
    simh.add_log("root.u_filter.o_signal", "root.clk", "p", 0, 1, 0, "t")
    simh.add_log("root.u_filter.o_shift_reg", "root.clk", "p", 0, 1, 0, "t")
    simh.add_log("root.u_filter.coeffs", "root.clk", "p", 0, 1, 1, "t")

    # # Sine generator
    simh.add_log("root.u_sin_gen.o_sin", "root.clk", "p", 0, 1, 0, "t")

################################################################################
# SETTINGS: SET COMMANDS
################################################################################

if args.run:
    pass

################################################################################
# SETTINGS: UPDATE YAML FILE
################################################################################

simh.update_settings_section()

# @ AUTO_SETTINGS_BEGIN

simh.settings['root']['fs_hz'] = 100
simh.settings['root']['logger_buffer_size'] = 1000
simh.settings['root']['n_iterations'] = 10000
simh.settings['root']['u_filter']['coeffs'] = [1, 1, 1, 1, 1]
simh.settings['root']['u_sin_gen']['amplitude_v'] = 1
simh.settings['root']['u_sin_gen']['frequency_hz'] = 10
simh.settings['root']['u_sin_gen']['phase_deg'] = 0

# @ AUTO_SETTINGS_END

simh.save_settings_yaml()
    
################################################################################
# RUN SIMULATOR
################################################################################

if args.run:
    simh.run()

################################################################################
# RUN TESTS
################################################################################

if args.run_tests:
    simh.run_tests()