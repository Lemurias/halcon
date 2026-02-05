#!/usr/bin/python3
################################################################################
# ██████████████████████████████████████████████████████████████████████████████
# █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
# █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
# █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
# █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
# █    ███    ██▌          ██▌   ███████▄   ▀██▀   ███    ███    ███   ██▄     █
# █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
# ██████████████████████████████████████████████████████████████████████████████
# █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
# ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
################################################################################
# Author: Patricio Reus Merlo
# Date: 06/25/2024
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
# HALCON MODULES
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
simh.core_dir = "../release/"

################################################################################
# SETTINGS: FILES
################################################################################

simh.cmakelists_file = "CMakeLists.txt"
simh.command_file = "conf/command.cmd"
simh.settings_file = "conf/settings.yaml"
simh.hierarchy_file = "conf/hierarchy.txt"
simh.main_cpp_file = "src/main.cpp"
simh.main_py_file = "main.py"
simh.binary_file = "run/basic_sim"

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
# SETTINGS: UPDATE YAML FILE
################################################################################

simh.update_settings_section()

# @ AUTO_SETTINGS_BEGIN

simh.settings['root']['br'] = '200e9'
simh.settings['root']['enable_log'] = 0
simh.settings['root']['logger_buffer_size'] = 1000
simh.settings['root']['m_qam'] = 16
simh.settings['root']['n_ovr'] = 2
simh.settings['root']['n_symbols'] = 10000
simh.settings['root']['u_awgn_ch']['ebno_db'] = 100
simh.settings['root']['u_awgn_ch']['seed'] = 0
simh.settings['root']['u_ber_counter']['corr_signals_size'] = 100
simh.settings['root']['u_ber_counter']['enable'] = 0
simh.settings['root']['u_downsampler']['phase'] = 0
simh.settings['root']['u_symbol_gen']['seed'] = 0
simh.settings['root']['u_upsampler']['phase'] = 0

# @ AUTO_SETTINGS_END

simh.save_settings_yaml()

################################################################################
# SETTINGS: LOG COMMANDS
################################################################################

if args.run:
    simh.clean_command_file()

    # LOGs @ root.clk_ch
    simh.add_log(signals = "root.clk_ch", 
                 clock = "root.clk_ch", 
                 edge = simh.POSITIVE,
                 begin = 0, step = 1, end = 0, 
                 file_type = simh.BINARY,
                 file_name_type = simh.LONG)

    # LOGs @ root.clk_br_tx
    log_signals = [
        # Clock
        "root.clk_br_tx",
        
        # Symbol Generator
        "root.u_symbol_gen.o_symb_tx",
        "root.u_symbol_gen.o_symb_ref",
    ]

    simh.add_log(signals = log_signals, 
                 clock = "root.clk_br_tx", 
                 edge = simh.POSITIVE,
                 begin = 0, step = 1, end = 0, 
                 file_type = simh.BINARY,
                 file_name_type = simh.LONG)
    
    # LOGs @ root.clk_br_rx
    log_signals = [
        # Clock
        "root.clk_br_rx",
        
        # Downsampler
        "root.u_downsampler.o_signal",

        # Slicer
        "root.u_slicer.o_signal",
        "root.u_slicer.i_signal",
        "root.u_slicer.o_error",
    ]

    simh.add_log(signals = log_signals, 
                 clock = "root.clk_br_rx", 
                 edge = simh.POSITIVE,
                 begin = 0, step = 1, end = 0, 
                 file_type = simh.BINARY,
                 file_name_type = simh.LONG)


################################################################################
# SETTINGS: SET COMMANDS
################################################################################

if args.run:    
    # BER Counter
    simh.add_setv(1, "root.u_ber_counter.enable", "root.clk_br_rx", 'p', 0, 0)
        
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