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
simh.modules_dir = "../lib/modules/"
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
simh.binary_file = "run/debug_sim_tp5"

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

    external_modules = ["symbol_generator",
                        "upsampler"]
    simh.add_external_modules(external_modules)
    
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

simh.update_hierarchy_section()

# @ AUTO_HIERARCHY_BEGIN

# -- ONLY LOGGEABLE SIGNALS --
#
#  - root.clk
#  - root.clk_cmd_handler
#  - root.clk_oversampling
#  - root.u_rc_filter.i_signal
#  - root.u_rc_filter.o_shift_reg
#  - root.u_rc_filter.o_signal
#  - root.u_symbol_gen.o_symb_ref
#  - root.u_symbol_gen.o_symb_tx
#  - root.u_upsampling.i_clock
#  - root.u_upsampling.i_n_ovr
#  - root.u_upsampling.i_signal
#  - root.u_upsampling.o_signal
#
# -- LOGGEABLE AND SETTABLE SIGNALS --
#
#  - root.baud_frequency_hz
#  - root.beta
#  - root.clk.frequency_hz
#  - root.clk.initial_phase
#  - root.clk_cmd_handler.frequency_hz
#  - root.clk_cmd_handler.initial_phase
#  - root.clk_oversampling.frequency_hz
#  - root.clk_oversampling.initial_phase
#  - root.logger_max_buffer_size
#  - root.m_qam
#  - root.n_iterations
#  - root.normalize
#  - root.oversampling
#  - root.p_tx
#  - root.u_rc_filter.coeffs
#  - root.u_rc_filter.r_out.i
#  - root.u_rc_filter.r_out.o
#  - root.u_rc_filter.r_shift_reg.i
#  - root.u_rc_filter.r_shift_reg.o
#  - root.u_symbol_gen.k
#  - root.u_symbol_gen.m_qam
#  - root.u_symbol_gen.p_qam
#  - root.u_symbol_gen.p_tx
#  - root.u_symbol_gen.r_symb_ref.i
#  - root.u_symbol_gen.r_symb_ref.o
#  - root.u_symbol_gen.r_symb_tx.i
#  - root.u_symbol_gen.r_symb_tx.o
#  - root.u_symbol_gen.seed
#  - root.u_upsampling.n_ovr
#  - root.u_upsampling.phase
#  - root.u_upsampling.r_counter.i
#  - root.u_upsampling.r_counter.o
#  - root.u_upsampling.r_out.i
#  - root.u_upsampling.r_out.o

# @ AUTO_HIERARCHY_END

################################################################################
# SETTINGS: LOG COMMANDS
################################################################################

if args.run:
    simh.clean_command_file()
    
    # # Clock 
    simh.add_log("root.clk", "root.clk_oversampling", "p", 0, 0, "txt")
    simh.add_log("root.clk.frequency_hz", "root.clk_oversampling", "p", 0, 1, "txt")

    # # Clock over sampling
    simh.add_log("root.clk_oversampling", "root.clk_oversampling", "p", 0, 0, "txt")
    simh.add_log("root.clk_oversampling.frequency_hz", "root.clk_oversampling", "p", 0, 1, "txt")

    # # Upsampling
    simh.add_log("root.u_upsampling.i_signal", "root.clk", "p", 0, 0, "txt")
    simh.add_log("root.u_upsampling.o_signal", "root.clk_oversampling", "p", 0, 0, "txt")
    
    # # Filter
    simh.add_log("root.u_rc_filter.o_signal", "root.clk_oversampling", "p", 0, 0, "txt")
    simh.add_log("root.u_rc_filter.o_shift_reg", "root.clk_oversampling", "p", 0, 0, "txt")
    simh.add_log("root.u_rc_filter.coeffs", "root.clk_oversampling", "p", 0, 1, "txt")


################################################################################
# SETTINGS: SET COMMANDS
################################################################################

if args.run:
    pass
    # simh.add_setv(0, "root.u_sin_gen.amplitude_v", "root.clk_oversampling", "p", 10, 0)
    
################################################################################
# SETTINGS: UPDATE YAML FILE
################################################################################

simh.update_settings_section()

# @ AUTO_SETTINGS_BEGIN

simh.settings['root']['baud_frequency_hz'] = 250000.0
simh.settings['root']['beta'] = 0.5
simh.settings['root']['logger_max_buffer_size'] = 10000
simh.settings['root']['m_qam'] = 4
simh.settings['root']['n_iterations'] = 10000
simh.settings['root']['normalize'] = 1
simh.settings['root']['oversampling'] = 4
simh.settings['root']['p_tx'] = 1

simh.settings['root']['clk']['frequency_hz'] = 250000
simh.settings['root']['clk']['initial_phase'] = 0

simh.settings['root']['clk_cmd_handler']['frequency_hz'] = 1000000
simh.settings['root']['clk_cmd_handler']['initial_phase'] = 0

simh.settings['root']['clk_oversampling']['frequency_hz'] = 1000000
simh.settings['root']['clk_oversampling']['initial_phase'] = 0

simh.settings['root']['u_rc_filter']['coeffs'] = '0,0,0,0,0,0,0,0,0,0,0,0,0,0,0'

simh.settings['root']['u_symbol_gen']['seed'] = 0

simh.settings['root']['u_upsampling']['phase'] = 0

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