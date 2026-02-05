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

import numpy as np
import os
import matplotlib.pyplot as plt

from matplotlib.gridspec import GridSpec

################################################################################
# HALCON MODULES
################################################################################

from halcon import Options
from halcon import SimulatorHandler
from halcon import Parameter
from halcon import Case
from halcon import Test
from halcon import Processor
from halcon import Beamer, NoEscape, bold, italic

import plotter
import dsp

################################################################################
# COMMAND LINE OPTIONS
################################################################################

args = Options.args()

################################################################################
# SIMULATOR HANDLER
################################################################################

simh = SimulatorHandler()

#######################################
# HANDLER: DIRECTORIES
#######################################

simh.project_abs_dir = os.getcwd() + "/../../"
simh.build_dir = "build/"
simh.conf_dir = "conf/"
simh.run_dir = "run/"
simh.logs_dir = "run/logs/"
simh.src_dir = "src/"
simh.tests_dir = "tests/"
simh.modules_dir = None
simh.core_dir = "../release/"

#######################################
# HANDLER: FILES
#######################################

simh.cmakelists_file = "CMakeLists.txt"
simh.command_file = "conf/command.cmd"
simh.settings_file = "conf/settings.yaml"
simh.hierarchy_file = "conf/hierarchy.txt"
simh.main_cpp_file = "src/main.cpp"
simh.main_py_file = "tests/ber_vs_ebno/sweep.py"
simh.binary_file = "run/basic_sim"

#######################################
# HANDLER: INIT
#######################################

simh.init()

#######################################
# HANDLER: COMPILATION
#######################################

if args.delete:
    simh.delete_old_files()

if args.enable_logs:
    simh.enable_compilation_logs()

if args.compile_f or args.compile_p:
    simh.set_make_parallelism(12)
    
    # Local modules
    local_modules = simh.get_available_local_modules()
    simh.add_local_modules(local_modules)

if args.compile_f:
    simh.compile_full()

if args.compile_p:
    simh.compile_posedge()

#######################################
# HANDLER: HIERARCHY
#######################################

simh.update_hierarchy_section()

# @ AUTO_HIERARCHY_BEGIN

# -- ONLY LOGGEABLE SIGNALS --
#
#  - root.clk
#  - root.clk_cmd_handler
#  - root.u_ber_counter.i_clock
#  - root.u_ber_counter.i_m_qam
#  - root.u_ber_counter.i_symb_hat
#  - root.u_ber_counter.i_symb_ref
#  - root.u_ber_counter.o_ber_value
#  - root.u_ber_counter.o_n_bits
#  - root.u_ber_counter.o_n_errors
#  - root.u_channel.i_clock
#  - root.u_channel.i_p_tx
#  - root.u_channel.i_signal
#  - root.u_channel.o_signal
#  - root.u_channel.u_awgn_ch.i_clock
#  - root.u_channel.u_awgn_ch.i_m_qam
#  - root.u_channel.u_awgn_ch.i_n_ovr
#  - root.u_channel.u_awgn_ch.i_p_tx
#  - root.u_channel.u_awgn_ch.i_signal
#  - root.u_channel.u_awgn_ch.o_signal
#  - root.u_receiver.i_clock
#  - root.u_receiver.i_m_qam
#  - root.u_receiver.i_p_rx
#  - root.u_receiver.i_signal
#  - root.u_receiver.o_signal
#  - root.u_receiver.u_slicer.i_signal
#  - root.u_receiver.u_slicer.o_error
#  - root.u_receiver.u_slicer.o_signal
#  - root.u_transmitter.o_symb_ref
#  - root.u_transmitter.o_tx
#  - root.u_transmitter.u_symbol_gen.o_symb_ref
#  - root.u_transmitter.u_symbol_gen.o_symb_tx
#
# -- LOGGEABLE AND SETTABLE SIGNALS --
#
#  - root.clk.frequency_hz
#  - root.clk.initial_phase
#  - root.clk_cmd_handler.frequency_hz
#  - root.clk_cmd_handler.initial_phase
#  - root.enable_log
#  - root.logger_max_buffer_size
#  - root.m_qam
#  - root.n_iterations
#  - root.n_ovr
#  - root.p_tx
#  - root.u_ber_counter.corr_signals_size
#  - root.u_ber_counter.enable
#  - root.u_ber_counter.phase
#  - root.u_ber_counter.r_ber_value.i
#  - root.u_ber_counter.r_ber_value.o
#  - root.u_ber_counter.r_delay_comp.i
#  - root.u_ber_counter.r_delay_comp.o
#  - root.u_ber_counter.r_n_bits.i
#  - root.u_ber_counter.r_n_bits.o
#  - root.u_ber_counter.r_n_errors.i
#  - root.u_ber_counter.r_n_errors.o
#  - root.u_ber_counter.valid_phase
#  - root.u_channel.u_awgn_ch.ebno_db
#  - root.u_channel.u_awgn_ch.m_qam
#  - root.u_channel.u_awgn_ch.n_ovr
#  - root.u_channel.u_awgn_ch.p_noise
#  - root.u_channel.u_awgn_ch.p_tx
#  - root.u_channel.u_awgn_ch.r_out.i
#  - root.u_channel.u_awgn_ch.r_out.o
#  - root.u_channel.u_awgn_ch.seed
#  - root.u_channel.u_awgn_ch.snr_lin
#  - root.u_receiver.r_slicer.i
#  - root.u_receiver.r_slicer.o
#  - root.u_receiver.u_slicer.decision
#  - root.u_receiver.u_slicer.error
#  - root.u_receiver.u_slicer.k
#  - root.u_receiver.u_slicer.m_qam
#  - root.u_transmitter.u_symbol_gen.k_qam
#  - root.u_transmitter.u_symbol_gen.m_qam
#  - root.u_transmitter.u_symbol_gen.p_qam
#  - root.u_transmitter.u_symbol_gen.p_tx
#  - root.u_transmitter.u_symbol_gen.r_symb_ref.i
#  - root.u_transmitter.u_symbol_gen.r_symb_ref.o
#  - root.u_transmitter.u_symbol_gen.r_symb_tx.i
#  - root.u_transmitter.u_symbol_gen.r_symb_tx.o
#  - root.u_transmitter.u_symbol_gen.seed

# @ AUTO_HIERARCHY_END

#######################################
# HANDLER: SET AND LOG COMMANDS
#######################################

begin = 0
end = 100000

simh.clean_command_file()

# # Clocks
# simh.add_log("root.clk", "root.clk", "p", begin, 1, end, "txt")

# # Symbol Generator
# simh.add_log("root.u_transmitter.u_symbol_gen.o_symb_tx", "root.clk", "p", begin, 1, end, "txt")
# simh.add_log("root.u_transmitter.u_symbol_gen.o_symb_ref", "root.clk", "p", begin, 1, end, "txt")

# # Channel
# simh.add_log("root.u_channel.u_awgn_ch.o_signal", "root.clk", "p", begin, 1, end, "txt")

# # Slicer
# simh.add_log("root.u_receiver.u_slicer.i_signal", "root.clk", "p", begin, 1, end, "txt")
# simh.add_log("root.u_receiver.u_slicer.o_signal", "root.clk", "p", begin, 1, end, "txt")
# simh.add_log("root.u_receiver.u_slicer.o_error", "root.clk", "p", begin, 1, end, "txt")

# # BER Counter
# simh.add_log("root.u_ber_counter.enable", "root.clk", "p", begin, 1, end, "txt")
# simh.add_log("root.u_ber_counter.phase", "root.clk", "p", begin, 1, end, "txt")
# simh.add_log("root.u_ber_counter.o_ber_value", "root.clk", "p", begin, 1, end, "txt")

simh.add_final_log("root.u_ber_counter.o_ber_value", "bin")

simh.add_setv(1, "root.u_ber_counter.enable", "root.clk", "p", 0, 0)

#######################################
# HANDLER: BASE SETTINGS
#######################################

simh.update_settings_section()

# @ AUTO_SETTINGS_BEGIN

simh.settings['root']['enable_log'] = 0
simh.settings['root']['logger_max_buffer_size'] = 1000
simh.settings['root']['m_qam'] = 16
simh.settings['root']['n_iterations'] = 5000000
simh.settings['root']['n_ovr'] = 1
simh.settings['root']['p_tx'] = 1

simh.settings['root']['clk']['frequency_hz'] = '200e9'
simh.settings['root']['clk']['initial_phase'] = 0

simh.settings['root']['clk_cmd_handler']['frequency_hz'] = '200e9'
simh.settings['root']['clk_cmd_handler']['initial_phase'] = 0

simh.settings['root']['u_ber_counter']['corr_signals_size'] = 100
simh.settings['root']['u_ber_counter']['enable'] = 0

simh.settings['root']['u_channel']['u_awgn_ch']['ebno_db'] = 18
simh.settings['root']['u_channel']['u_awgn_ch']['seed'] = 0

simh.settings['root']['u_transmitter']['u_symbol_gen']['seed'] = 0

# @ AUTO_SETTINGS_END

simh.save_settings_yaml()

#######################################
# HANDLER: RUN
#######################################

if args.run_sim:
    simh.run()

#######################################
# HANDLER: RUN
#######################################

if args.run_tests:
    simh.add_tests(["plot.py"])
    simh.run_tests()

################################################################################
# PARAMETERS
################################################################################

p_m_qam_db_v = Parameter (
    name = 'root.m_qam', 
    text = '{:.0f}_QAM',
    alpha = 1, 
    value = [4, 16, 64],
    simh = simh
)

p_ebno_db_v = Parameter (
    name = 'root.u_channel.u_awgn_ch.ebno_db', 
    text = 'ebno_{:.3f}_db',
    alpha = 1, 
    value = np.linspace(0, 15, 16),
    simh = simh
)

################################################################################
# TEST
################################################################################

test = Test (
    name = "Parameter Sweep",
    base_dir = "../scratch/",
    parameters = Parameter.list()
)

################################################################################
# CASES
################################################################################

#######################################
# CREATE CASES
#######################################

for p_m_qam_db in p_m_qam_db_v:
    for p_ebno_db in p_ebno_db_v:
                
        # Create Case
        case = Case(simh)
        
        # Add Parameters
        case.add(p_m_qam_db)
        case.add(p_ebno_db)

        # Add to Test
        case = test.add(case)

#######################################
# SAVE CASES
#######################################

test.save_cases()

################################################################################
# TEST OPTIONS
################################################################################

#######################################
# DELETE TEST
#######################################

if args.delete_test:
    test.delete()

#######################################
# RUN
#######################################

# 1) Remove old test folder if it exist
# 2) Create the test directory tree
# 3) Run all cases

if args.run_cases:
    test.run()

#######################################
# RUN AGAIN
#######################################

# 1) Use already created test
# 2) Run all cases again

if args.run_again:
    test.run_again()

#######################################
# RUN MISSING
#######################################

# 1) Use already created test
# 2) Run cloned but not finished cases again

if args.run_missing:
    test.run_missing()

#######################################
# SUMMARY
#######################################

if args.summary:
    test.summary()

#######################################
# DIRECTORIES
#######################################

if args.folders_list:
    test.folders_list()

#######################################
# LIVE
#######################################

if args.live:
    test.live()

#######################################
# KILL
#######################################

if args.kill:
    test.kill()

################################################################################
# PROCESSING
################################################################################

if args.processing:

    ###################################
    # OUTPUT DIRECTORY
    ###################################

    img_dir = "./img"
    img_dir = os.path.abspath(img_dir)

    os.makedirs(img_dir, exist_ok=True)

    ###################################
    # DATA PROCESSOR
    ###################################

    p = Processor(test.directory)
    
    ###################################
    # AXIS
    ###################################

    m_qam_v = p.axis("root.m_qam")
    ebno_db_v = p.axis("root.u_channel.u_awgn_ch.ebno_db")

    n_ebno = len(ebno_db_v)
    n_m_qam = len(m_qam_v)

    theo_ber_m = np.empty((n_m_qam, n_ebno))
    sim_ber_m = np.empty((n_m_qam, n_ebno))

    for i in range(n_m_qam):
        theo_ber_m[i, :] = dsp.qam_ber_awgn(ebno_db_v, m_qam_v[i])
    
    ###################################
    # LOAD DATA
    ###################################

    data = p.signals()

    ###################################
    # PLOTS
    ###################################

    img_names = []
    xlim = (-2, 2)
    ylim = (-2, 2)

    for i, m_qam in enumerate(m_qam_v):
        for j, ebno_db in enumerate(ebno_db_v):
        
            ###############################
            # BER
            ###############################

            case_ber = data[i, j]["root.u_ber_counter.o_ber_value"]['s'][-1]
            sim_ber_m[i, j] = case_ber

            continue

            ###############################
            # FIGURE SETTINGS
            ###############################

            plotter.set_default_settings(30)

            fig = plt.figure(figsize=(32, 8))
            gs = GridSpec(1, 4)

            ax1 = fig.add_subplot(gs[0, 0])
            ax2 = fig.add_subplot(gs[0, 1])
            ax3 = fig.add_subplot(gs[0, 2])
            ax4 = fig.add_subplot(gs[0, 3])

            ###############################
            # SYMBOL GENERATOR
            ###############################

            delay = 0
            ticks = 1000
            x0 = delay
            x1 = ticks + delay

            clk = data[i, j]["root.clk"]['s'][x0 : x1]
            o_symb_tx = data[i, j]["root.u_transmitter.u_symbol_gen.o_symb_tx"]['s'][x0 : x1]

            tit = r"Symbol Generator"
            xlab = r"In-phase"
            ylab = r"Quadrature"

            ax1.scatter(o_symb_tx.real, 
                        o_symb_tx.imag, 
                        color='k', 
                        s=80)

            ax1.set_title(tit)
            ax1.set_xlabel(xlab)
            ax1.set_ylabel(ylab)
            ax1.set_xlim(xlim)
            ax1.set_ylim(ylim)

            ###############################
            # AWGN CHANNEL OUTPUT
            ###############################

            o_channel_awgn = data[i, j]["root.u_channel.u_awgn_ch.o_signal"]['s']

            plotter.scatter_density(axis=ax2,
                                    signal=o_channel_awgn,
                                    bins=100,
                                    cmap='hot')

            tit = r"AWGN Channel"
            xlab = r"In-phase"
            ylab = r"Quadrature"

            ax2.set_title(tit)
            ax2.set_xlabel(xlab)
            ax2.set_ylabel(ylab)
            ax2.set_facecolor('k')
            ax2.set_xlim(xlim)
            ax2.set_ylim(ylim)

            ###############################
            # SLICER (evolution)
            ###############################

            clk_time = data[i, j]["root.clk"]['s']
            o_slicer = data[i, j]["root.u_receiver.u_slicer.o_signal"]['s']
            i_slicer = data[i, j]["root.u_receiver.u_slicer.i_signal"]['s']

            n_delay = 1
            i_slicer = np.array(i_slicer)
            i_slicer = np.roll(i_slicer, n_delay)
            i_slicer[:n_delay] = np.NaN

            ax3.scatter(clk_time / 1e-9, i_slicer.real, color='r', s=50, alpha=0.1)
            ax3.scatter(clk_time / 1e-9, o_slicer.real, color='b', marker='x', linewidths=1, s=20)

            tit = r"Slicer Input vs Output (real)"
            xlab = r"Time [nsec]"
            ylab = r"Amplitude"

            ax3.set_title(tit)
            ax3.set_xlabel(xlab)
            ax3.set_ylabel(ylab)
            ax3.set_ylim(np.log2(m_qam) * np.array([-1, 1]) + np.array([-2, 2]))

            ###############################
            # SLICER
            ###############################

            min_time = 10e-9
            max_time = 10.15e-9

            clk_time = data[i, j]["root.clk"]['s']
            clk_time, x0, x1 = dsp.trimmer(clk_time, min_time, max_time)

            o_slicer = data[i, j]["root.u_receiver.u_slicer.o_signal"]['s']
            i_slicer = data[i, j]["root.u_receiver.u_slicer.i_signal"]['s']
            o_slicer_error = data[i, j]["root.u_receiver.u_slicer.o_error"]['s']
            o_symb_ref = data[i, j]["root.u_transmitter.u_symbol_gen.o_symb_ref"]['s']

            n_delay = 2
            o_symb_ref = np.array(o_symb_ref)
            o_symb_ref = np.roll(o_symb_ref, n_delay)
            o_symb_ref[:n_delay] = np.NaN

            n_delay = 0
            i_slicer = np.array(i_slicer)
            i_slicer = np.roll(i_slicer, n_delay)
            i_slicer[:n_delay] = np.NaN

            o_slicer = o_slicer[x0 : x1]
            i_slicer = i_slicer[x0 : x1]
            o_slicer_error = o_slicer_error[x0 : x1]
            o_symb_ref = o_symb_ref[x0 : x1]

            label_in = r"Input"
            label_out = r"Output"
            label_ref = r"Reference"
            label_err = r"Error"

            ax4.scatter(clk_time / 1e-9, i_slicer.real, color='r', s=100, label=label_in)
            ax4.scatter(clk_time / 1e-9, o_slicer.real, color='b', marker='x',linewidths=4, label=label_out)
            ax4.scatter(clk_time / 1e-9, o_symb_ref.real, 
                        color='k',
                        marker='s',
                        edgecolors='black',
                        facecolors='none',
                        linewidths=2,
                        s=200,
                        label=label_ref)

            ax4.scatter(clk_time / 1e-9, o_slicer_error.real, color='k', s=100, label=label_err)
            tit = r"Slicer Comparison (real)"
            xlab = r"Time [nsec]"
            ylab = r"Amplitude"

            ax4.set_title(tit)
            ax4.set_xlabel(xlab)
            ax4.set_ylabel(ylab)
            ax4.set_ylim([x * 1.2 for x in ax4.get_ylim()])
            ax4.legend(loc='upper right', ncol=1)

            ###############################
            # SAVE AND CLOSE PLOT
            ###############################

            img_name = os.path.join(img_dir, f'sweep_{i}_{j}.png')
            img_names.append(img_name)
            plt.savefig(os.path.join(img_dir, img_name))
            plt.close()

    ###############################
    # FIGURE SETTINGS
    ###############################

    plotter.set_default_settings(25)
    fig = plt.figure(figsize=(10, 8))

    colors = ['r', 'g', 'b']

    for i, m_qam in enumerate(m_qam_v):

        theo_ber_v = theo_ber_m[i, :] 
        sim_ber_v = sim_ber_m[i, :]

        plt.semilogy(ebno_db_v, theo_ber_v,'--', label='Theoretical', color=colors[i])
        plt.semilogy(ebno_db_v, sim_ber_v,'o', label=f'Sim QAM {m_qam}', color=colors[i])
        plt.title("BER vs EbNo")
        plt.xlabel("EbNo [dB]")
        plt.ylabel("BER")
        plt.grid(True, which="both")
        plt.legend(loc='lower left')

    ber_img = os.path.join(img_dir, "ber.png")
    plt.savefig(os.path.join(img_dir, ber_img))

################################################################################
# SLIDES
################################################################################

if args.processing and args.latex:
    
    ###################################
    # PREAMBLE
    ###################################

    # Create Beamer
    doc = Beamer()

    # Theme
    doc.theme("Madrid", "default")

    # Packages
    doc.package('lipsum')

    # Title
    doc.preamble_line_comment()
    doc.title("QAM Basic Simulator", "HALCON")
    doc.subtitle("BER vs EbNo - Sanity Check")
    doc.author("Patricio Reus Merlo", "P.ReusMerlo")
    doc.institute("Fundación Fulgor")
    doc.date()

    ###################################
    # TITLE PAGE
    ###################################

    with doc.create_frame() as frame:
        frame.title_page()

    ###################################
    # FRAME: BER
    ###################################

    with doc.create_frame(title=f"BER vs EbNo") as frame:
        with doc.create_figure() as fig:
                path = NoEscape(os.path.relpath(ber_img))
                width = NoEscape(r'0.5\textwidth')
                fig.add_image(path, width=width)


    ###################################
    # FRAMES: PLOTS
    ###################################

    ziped_data = zip(test.case_names, test.case_descriptions, img_names)
    for case_name, case_desc, case_img in ziped_data:

        with doc.create_frame(title=f"Case {case_name}") as frame:

            doc.append(NoEscape(bold("Description: ") + italic(case_desc)))

            with doc.create_figure() as fig:
                path = NoEscape(os.path.relpath(case_img))
                width = NoEscape(r'\textwidth')
                fig.add_image(path, width=width)

    ###################################
    # SAVE
    ###################################

    doc.generate('slides')