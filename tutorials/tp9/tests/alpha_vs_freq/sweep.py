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
# Date: 07/03/2024
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
import scipy as sp
import os
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

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
simh.modules_dir = "../lib/modules/"
simh.core_dir = "../lib/core/"

#######################################
# HANDLER: FILES
#######################################

simh.cmakelists_file = "CMakeLists.txt"
simh.command_file = "conf/command.cmd"
simh.settings_file = "conf/settings.yaml"
simh.hierarchy_file = "conf/hierarchy.txt"
simh.main_cpp_file = "src/main.cpp"
simh.main_py_file = "tests/alpha_vs_freq/sweep.py"
simh.binary_file = "run/sim_sweep"

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

    # External modules
    external_modules = ["symbol_generator"]
    
    simh.add_external_modules(external_modules)

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
#  - root.u_filter_iir.i_signal
#  - root.u_filter_iir.o_signal
#  - root.u_symbol_gen.o_symb_ref
#  - root.u_symbol_gen.o_symb_tx
#
# -- LOGGEABLE AND SETTABLE SIGNALS --
#
#  - root.clk.frequency_hz
#  - root.clk.initial_phase
#  - root.clk_cmd_handler.frequency_hz
#  - root.clk_cmd_handler.initial_phase
#  - root.enable_log
#  - root.iteration_rate
#  - root.logger_max_buffer_size
#  - root.n_iterations
#  - root.u_filter_iir.alpha
#  - root.u_filter_iir.r_out.i
#  - root.u_filter_iir.r_out.o
#  - root.u_filter_iir.y_t
#  - root.u_symbol_gen.k
#  - root.u_symbol_gen.m_qam
#  - root.u_symbol_gen.p_qam
#  - root.u_symbol_gen.p_tx
#  - root.u_symbol_gen.r_symb_ref.i
#  - root.u_symbol_gen.r_symb_ref.o
#  - root.u_symbol_gen.r_symb_tx.i
#  - root.u_symbol_gen.r_symb_tx.o
#  - root.u_symbol_gen.seed

# @ AUTO_HIERARCHY_END

#######################################
# HANDLER: SET AND LOG COMMANDS
#######################################

begin = 0
end = 10000

simh.clean_command_file()


# # Clocks
simh.add_log("root.clk", "root.clk", "p", 0, 0, "bin")
simh.add_log("root.clk.frequency_hz", "root.clk", "p", 0, 1, "bin")

# # Symbol generator
simh.add_log("root.u_symbol_gen.o_symb_ref", "root.clk", "p", 0, 0, "bin")

# # Filter
simh.add_log("root.u_filter_iir.i_signal", "root.clk", "p", 0, 0, "bin")
simh.add_log("root.u_filter_iir.o_signal", "root.clk", "p", 0, 0, "bin")
simh.add_log("root.u_filter_iir.alpha", "root.clk", "p", 0, 1, "bin")



#######################################
# HANDLER: BASE SETTINGS
#######################################

simh.update_settings_section()

# @ AUTO_SETTINGS_BEGIN

simh.settings['root']['enable_log'] = 1
simh.settings['root']['logger_max_buffer_size'] = 1000
simh.settings['root']['n_iterations'] = 100000

simh.settings['root']['clk']['frequency_hz'] = 100000.0
simh.settings['root']['clk']['initial_phase'] = 0

simh.settings['root']['clk_cmd_handler']['frequency_hz'] = 100000.0
simh.settings['root']['clk_cmd_handler']['initial_phase'] = 0

simh.settings['root']['u_filter_iir']['alpha'] = 0.01

simh.settings['root']['u_symbol_gen']['seed'] = 0

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

p_alpha_v = Parameter (
    name = 'root.u_filter_iir.alpha', 
    text = 'alpha_{:.2f}',
    alpha = 1, 
    value = [0.01, 0.05, 0.10, 0.15, 0.20],
    simh = simh
)

p_sampling_frequency_v = Parameter (
    name = 'root.clk.frequency_hz', 
    text = 'samp_frq_{:.0f}_khz',
    alpha = 1/1e3, 
    value = np.linspace(25e3, 100e3, 4),
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

for p_alpha in p_alpha_v:
    for p_sampling_frequency in p_sampling_frequency_v:
        
        # Create Case
        case = Case(simh)
                    
        # Add Parameters
        case.add(p_alpha)
        case.add(p_sampling_frequency)

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

    alpha_v = p.axis("root.u_filter_iir.alpha")
    sampling_frequency_v = p.axis("root.clk.frequency_hz")
    
    ###################################
    # LOAD DATA
    ###################################

    data = p.signals()

    ###################################
    # PLOTS
    ###################################

    img_names = []
    # xlim = (-2, 2)
    # ylim = (-2, 2)
    formatter = ScalarFormatter()
    formatter.set_scientific(True)
    formatter.set_powerlimits((0, 0))

    plt.tight_layout()

    for i, alpha in enumerate(alpha_v):
        for j, sampling_frequency in enumerate(sampling_frequency_v):
                        
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

                        ############################################
                        # SUBPLOT 1
                        ############################################

                        ###############################
                        # FILTER INPUT
                        ###############################

                        delay = 0
                        ticks = 100
                        x0 = delay
                        x1 = ticks + delay

                        tit = r"Filter Input"
                        xlab = r"Time [sec]"
                        ylab = r"Amplitude [V]"

                        i_filter = data[i,j]["root.u_filter_iir.i_signal"]['s'][x0 : x1]
                        clk = data[i,j]["root.clk"]['s'][x0 : x1]

                        ax1.stem(clk, np.real(i_filter), label='root.u_filter_iir.i_signal')

                        ax1.set_title(tit)
                        ax1.set_xlabel(xlab)
                        ax1.set_ylabel(ylab)
                        # ax1.set_xlim(xlim)
                        # ax1.set_ylim(ylim)

                        ############################################
                        # SUBPLOT 2
                        ############################################

                        ###############################
                        # FILTER OUTPUT
                        ###############################

                        tit = r"Filter Output"
                        xlab = r"Time [sec]"
                        ylab = r"Amplitude [V]"

                        o_filter = data[i,j]["root.u_filter_iir.o_signal"]['s'][x0 : x1]
                        clk = data[i,j]["root.clk"]['s'][x0 : x1]

                        ax2.plot(clk, np.real(o_filter), 'g', label='u_filter_iir.o_signal')

                        ax2.set_title(tit)
                        ax2.set_xlabel(xlab)
                        ax2.set_ylabel(ylab)
                        ax2.set_facecolor('k')
                        # ax2.set_xlim(xlim)
                        # ax2.set_ylim(ylim)

                        ############################################
                        # SUBPLOT 3
                        ############################################

                        ###############################
                        # FFT - FILTER INPUT
                        ###############################

                        i_signal = data[i,j]["root.u_filter_iir.i_signal"]['s']

                        # Fast Fourier Transform
                        signal = np.array(i_signal)
                        fourier = np.fft.fft(signal, n=len(signal))
                        n = (signal.size)
                        fs = data[i,j]["root.clk.frequency_hz"]['s'][0]
                        timestep = 1/fs
                        freq = np.fft.fftfreq(n, d=timestep)

                        freq_pxx, pxx = sp.signal.welch(signal, fs=fs, nperseg=1024, noverlap=0, nfft=None, detrend='constant', return_onesided=False, scaling='density', axis=-1, average='mean')

                        # Convertion to dBm
                        mag_H_pxx = np.abs(pxx)/(np.max(pxx))
                        # Avoid zeros to avoid division by zero
                        mag_H_pxx[mag_H_pxx == 0] = 1e-15
                        mag_H_dbm_pxx = 10 * np.log10(mag_H_pxx)

                        ax3.plot(freq_pxx, mag_H_dbm_pxx, label='Filter Input PSD')

                        tit = r"Filter Response & PSD"
                        xlab = r"Frequency [Hz]"
                        ylab = r"Power [dBm]"   

                        ax3.set_title(tit)
                        ax3.set_xlabel(xlab)
                        ax3.set_ylabel(ylab)

                        ###############################
                        # FFT - FILTER OUTPUT
                        ###############################
 
                        o_signal = data[i,j]["root.u_filter_iir.o_signal"]['s']

                        # Power Spectral Density
                        signal = np.array(o_signal)
                        freq_pxx, pxx = sp.signal.welch(signal, fs=fs, nperseg=4096, noverlap=0, nfft=None, detrend='constant', return_onesided=False, scaling='density', axis=-1, average='mean')

                        # Convertion to dBm
                        mag_H_pxx = np.abs(pxx)/(np.max(pxx))
                        # Avoid zeros to avoid division by zero
                        mag_H_pxx[mag_H_pxx == 0] = 1e-15
                        mag_H_dbm_pxx = 10 * np.log10(mag_H_pxx)

                        ax3.plot(freq_pxx, mag_H_dbm_pxx, 'g', label='Filter Output PSD')

                        
                        ###############################
                        # FILTER
                        ###############################

                        alpha = data[i,j]["root.u_filter_iir.alpha"]['s'][0]

                        # IIR Filter Coeffs
                        b = [alpha]  # num coeffs
                        a = [1.0000, -(1-alpha)]  # den coeffs

                        # Sample frequency
                        fs = data[i,j]["root.clk.frequency_hz"]['s'][0]

                        # Frequency response calculation
                        w, h = sp.signal.freqz(b, a, worN=10000)

                        # Covert normalize frequency to Hz
                        frequencies = w * fs / (2 * np.pi)

                        frequencies_full = np.concatenate((-frequencies[::-1], frequencies))
                        h_full = np.concatenate((h[::-1], h))

                        # Convertion to dBm
                        mag_H = np.abs(h_full)**2
                        mag_H = mag_H/(np.max(mag_H))
                        # Avoid zeros to avoid division by zero
                        mag_H[mag_H == 0] = 1e-15
                        mag_H_dbm = 10 * np.log10(mag_H)

                        ax3.plot(frequencies_full, mag_H_dbm, 'r', label='Filter')
                        ax3.set_ylim([-45,5])
                        ax3.set_title(tit)
                        ax3.set_xlabel(xlab)
                        ax3.set_ylabel(ylab)
                        ax3.legend(loc='lower left', fontsize=16)
                        

                        ###############################
                        # IMPULSE RESPONSE - TAPS
                        ###############################

                        tit = r"Impulse Response"
                        xlab = r"Samples"
                        ylab = r"Amplitude"

                        b = [alpha]
                        a = [1.0, -(1.0-alpha)]

                        system = (b,a,1)
                        t, h_impulse = sp.signal.dimpulse(system)

                        ax4.stem(t, np.squeeze(h_impulse), 'r')
                        ax4.set_title(tit)
                        ax4.set_xlabel(xlab)
                        ax4.set_ylabel(ylab)

                        ###############################
                        # SAVE AND CLOSE PLOT
                        ###############################

                        img_name = os.path.join(img_dir, f'sweep_{i}_{j}.png')
                        img_names.append(img_name)
                        plt.savefig(os.path.join(img_dir, img_name))
                        plt.close()


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
    doc.title("IIR Filter", "HALCON")
    doc.subtitle("Alpha vs Frequency")
    doc.author("Manuel Exequiel Arias", "Manuel E. Arias")
    doc.institute("Fundación Fulgor")
    doc.date()

    ###################################
    # TITLE PAGE
    ###################################

    with doc.create_frame() as frame:
        frame.title_page()


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