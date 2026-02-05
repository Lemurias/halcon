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
# Date: 05/31/2024
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
simh.binary_file = "run/simulator"

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
    
    # external_modules = simh.get_available_external_modules()
    # external_modules = ["symbol_generator",
                        # "upsampler"]
    external_modules = ["symbol_generator"]
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
# SETTINGS: HIERARCHY
################################################################################

simh.update_hierarchy_section()

# @ AUTO_HIERARCHY_BEGIN

# -- ONLY LOGGEABLE SIGNALS --
#
#  - root.clk
#  - root.clk_cmd_handler
#  - root.clk_fast
#  - root.clk_slow
#  - root.u_filter.i_signal
#  - root.u_filter.o_signal
#  - root.u_par_to_ser.i_clock_fast
#  - root.u_par_to_ser.i_parallel
#  - root.u_par_to_ser.o_serial
#  - root.u_parallel_filter.i_signal
#  - root.u_parallel_filter.o_signal
#  - root.u_ser_to_par.i_clock_fast
#  - root.u_ser_to_par.i_clock_slow
#  - root.u_ser_to_par.i_serial
#  - root.u_ser_to_par.o_parallel
#  - root.u_symbol_gen.o_symb_ref
#  - root.u_symbol_gen.o_symb_tx
#  - root.u_upsampler.i_clock
#  - root.u_upsampler.i_n_ovr
#  - root.u_upsampler.i_signal
#  - root.u_upsampler.o_signal_i
#  - root.u_upsampler.o_signal_q
#
# -- LOGGEABLE AND SETTABLE SIGNALS --
#
#  - root.clk.frequency_hz
#  - root.clk.initial_phase
#  - root.clk_cmd_handler.frequency_hz
#  - root.clk_cmd_handler.initial_phase
#  - root.clk_fast.frequency_hz
#  - root.clk_fast.initial_phase
#  - root.clk_slow.frequency_hz
#  - root.clk_slow.initial_phase
#  - root.enable_log
#  - root.iteration_rate
#  - root.logger_max_buffer_size
#  - root.n_iterations
#  - root.n_ovr
#  - root.normalize
#  - root.u_filter.coeffs
#  - root.u_filter.r_out.i
#  - root.u_filter.r_out.o
#  - root.u_filter.r_shift_reg.i
#  - root.u_filter.r_shift_reg.o
#  - root.u_par_to_ser.r_counter.i
#  - root.u_par_to_ser.r_counter.o
#  - root.u_par_to_ser.r_out.i
#  - root.u_par_to_ser.r_out.o
#  - root.u_parallel_filter.coeffs
#  - root.u_parallel_filter.r_reg.i
#  - root.u_parallel_filter.r_reg.o
#  - root.u_parallel_filter.result
#  - root.u_ser_to_par.r_buffer.i
#  - root.u_ser_to_par.r_buffer.o
#  - root.u_ser_to_par.r_counter.i
#  - root.u_ser_to_par.r_counter.o
#  - root.u_symbol_gen.k
#  - root.u_symbol_gen.m_qam
#  - root.u_symbol_gen.p_qam
#  - root.u_symbol_gen.p_tx
#  - root.u_symbol_gen.r_symb_ref.i
#  - root.u_symbol_gen.r_symb_ref.o
#  - root.u_symbol_gen.r_symb_tx.i
#  - root.u_symbol_gen.r_symb_tx.o
#  - root.u_symbol_gen.seed
#  - root.u_upsampler.n_ovr
#  - root.u_upsampler.phase
#  - root.u_upsampler.r_counter.i
#  - root.u_upsampler.r_counter.o
#  - root.u_upsampler.r_out_i.i
#  - root.u_upsampler.r_out_i.o
#  - root.u_upsampler.r_out_q.i
#  - root.u_upsampler.r_out_q.o

# @ AUTO_HIERARCHY_END

################################################################################
# SETTINGS: LOG COMMANDS
################################################################################

if args.run:
    simh.clean_command_file()
    
    # Clocks
    simh.add_log("root.clk", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.clk_slow", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.clk_fast", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.clk_fast.frequency_hz", "root.clk_fast", "p", 0, 1, "txt")
    
    # Symbol generator
    simh.add_log("root.u_symbol_gen.o_symb_ref", "root.clk_fast", "p", 0, 0, "txt")

    # Up sampler
    simh.add_log("root.u_upsampler.o_signal_i", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_upsampler.o_signal_q", "root.clk_fast", "p", 0, 0, "txt")

    # Parallel Filter
    simh.add_log("root.u_parallel_filter.i_signal", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_parallel_filter.o_signal", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_parallel_filter.coeffs", "root.clk_fast", "p", 0, 1, "txt")
    simh.add_log("root.u_parallel_filter.result", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_parallel_filter.r_reg.i", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_parallel_filter.r_reg.o", "root.clk_fast", "p", 0, 0, "txt")

    # Serial Filter
    simh.add_log("root.u_filter.i_signal", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_filter.o_signal", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_filter.r_shift_reg.o", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_filter.coeffs", "root.clk_fast", "p", 0, 1, "txt")
    
    # Serial to Parallel
    simh.add_log("root.u_ser_to_par.i_serial", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_ser_to_par.o_parallel", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_ser_to_par.r_buffer.i", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_ser_to_par.r_buffer.o", "root.clk_fast", "p", 0, 0, "txt")

    # Parallel to Serial
    simh.add_log("root.u_par_to_ser.o_serial", "root.clk_fast", "p", 0, 0, "txt")
    simh.add_log("root.u_par_to_ser.i_parallel", "root.clk_fast", "p", 0, 0, "txt")
    
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

simh.settings['root']['enable_log'] = 0
simh.settings['root']['logger_max_buffer_size'] = 1000
simh.settings['root']['n_iterations'] = 10000
simh.settings['root']['n_ovr'] = 4
simh.settings['root']['normalize'] = 1

simh.settings['root']['clk']['frequency_hz'] = 25000.0
simh.settings['root']['clk']['initial_phase'] = 0

simh.settings['root']['clk_cmd_handler']['frequency_hz'] = 100000
simh.settings['root']['clk_cmd_handler']['initial_phase'] = 0

simh.settings['root']['clk_fast']['frequency_hz'] = 100000
simh.settings['root']['clk_fast']['initial_phase'] = 0

simh.settings['root']['clk_slow']['frequency_hz'] = 25000.0
simh.settings['root']['clk_slow']['initial_phase'] = 0

simh.settings['root']['u_filter']['coeffs'] = '0'

simh.settings['root']['u_parallel_filter']['coeffs'] = [0.25, 0.25, 0.25, 0.25]

simh.settings['root']['u_symbol_gen']['seed'] = 0

simh.settings['root']['u_upsampler']['phase'] = 0

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