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
# Author:
# Date:
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

###############################################################################
# POST SIMULATION PLOTS
###############################################################################

###############################################################################
# PYTHON MODULES
###############################################################################

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec

###############################################################################
# HALCON MODULES
###############################################################################

import dsp
import halcon
import plotter

###############################################################################
# LOAD SIGNALS
###############################################################################

data = halcon.Processor.load_directory("../run/logs/")

#######################################
# MATPLOTLIB GLOBAL SETTINGS
#######################################

plotter.set_default_settings(20)

#######################################
# FIGURE SETTINGS
#######################################

fig = plt.figure(figsize=(22, 20))
gs = GridSpec(4, 4)

ax1 = fig.add_subplot(gs[0,  0])
ax2 = fig.add_subplot(gs[0,  1])
ax3 = fig.add_subplot(gs[0,  2])
ax4 = fig.add_subplot(gs[0,  3])
ax5 = fig.add_subplot(gs[1,  0])
ax6 = fig.add_subplot(gs[1,  1])
ax7 = fig.add_subplot(gs[1,  2])
ax8 = fig.add_subplot(gs[1,  3])
ax9 = fig.add_subplot(gs[2,  0])
ax10 = fig.add_subplot(gs[2,  1])
ax11 = fig.add_subplot(gs[2,  2])
ax12 = fig.add_subplot(gs[2,  3])
ax13 = fig.add_subplot(gs[3,0:2])
ax14 = fig.add_subplot(gs[3,2:5])

#######################################
# SYMBOL GENERATOR
#######################################

delay = 0
ticks = 100
x0 = delay
x1 = ticks + delay

clk = data["root.clk"]['s'][x0 : x1]
o_symb_ref = data["root.u_transmitter.u_symbol_gen.o_symb_ref"]['s'][x0 : x1]

tit = r"Symbol Generator"
xlab = r"In-phase"
ylab = r"Quadrature"

ax1.scatter(o_symb_ref.real, 
            o_symb_ref.imag, 
            color='r', 
            s=80)

ax1.set_title(tit)
ax1.set_xlabel(xlab)
ax1.set_ylabel(ylab)
ax1.set_ylim([x * 1.4 for x in ax1.set_ylim()])
ax1.set_xlim([x * 1.2 for x in ax1.set_ylim()])

#######################################
# UPSAMPLER (INPUT VS OUTPUT)
#######################################

min_time = 0.1e-9
max_time = 0.2e-9

clk_time = data["root.clk"]['s']
clk_time, x0, x1 = dsp.trimmer(clk_time, min_time, max_time)

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

i_upsampler = data["root.u_transmitter.u_upsampler.i_signal"]['s'][x0 : x1]
o_upsampler = data["root.u_transmitter.u_upsampler.o_signal"]['s'][x0_os : x1_os]

n_delay = 1
i_upsampler = np.array(i_upsampler)
i_upsampler = np.roll(i_upsampler, n_delay)
i_upsampler[:n_delay] = np.NaN

tit = r"Upsampler (real)"
xlab = r"Time [nsec]"
ylab = r"Amplitude"
label_in = r"Input"
label_out = r"Output"

ax2.scatter(clk_time / 1e-9, i_upsampler.real, color='r', label=label_in)
ax2.scatter(clk_os_time / 1e-9, o_upsampler.real, color='b', label=label_out)

ax2.set_title(tit)
ax2.set_xlabel(xlab)
ax2.set_ylabel(ylab)
# ax2.set_ylim([x * 2 for x in ax2.set_ylim()])
ax2.legend(loc='upper center', ncol=2)

#######################################
# TX EYE DIAGRAM (filter output)
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_tx_filter = data["root.u_transmitter.u_tx_filter.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram(axis=ax3,
                    signal=o_tx_filter.real,
                    samples_per_symbol=4, 
                    span_symbols=2,
                    offset_samples=1,
                    color='b',
                    alpha=0.3,
                    linewidth=0.1)

tit = r"TX Filter Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax3.set_title(tit)
ax3.set_xlabel(xlab)
ax3.set_ylabel(ylab)

#######################################
# TX EYE DIAGRAM DENSITY
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_tx_filter = data["root.u_transmitter.u_tx_filter.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram_density(axis=ax4,
                            signal=o_tx_filter.real,
                            samples_per_symbol=4, 
                            span_symbols=2,
                            offset_samples=1,
                            interp_n = 1000,
                            bins=200,
                            cmap='hot')

tit = r"TX Filter Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax4.set_title(tit)
ax4.set_xlabel(xlab)
ax4.set_ylabel(ylab)
ax4.set_ylim(ax3.get_ylim())
ax4.set_facecolor('k')

#######################################
# CHANNEL (noise)
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_channel_awgn = data["root.u_channel.u_awgn_ch.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram(axis=ax5,
                    signal=o_channel_awgn.real,
                    samples_per_symbol=4, 
                    span_symbols=2,
                    offset_samples=2,
                    color='b',
                    alpha=0.3,
                    linewidth=0.1)

tit = r"AWGN Channel Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax5.set_title(tit)
ax5.set_xlabel(xlab)
ax5.set_ylabel(ylab)

#######################################
# CHANNEL (filter)
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_channel_filter = data["root.u_channel.u_ch_resp.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram(axis=ax6,
                    signal=o_channel_filter.real,
                    samples_per_symbol=4, 
                    span_symbols=2,
                    offset_samples=1,
                    color='b',
                    alpha=0.3,
                    linewidth=0.1)

tit = r"Channel Filter Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax6.set_title(tit)
ax6.set_xlabel(xlab)
ax6.set_ylabel(ylab)

#######################################
# CHANNEL EYE DIAGRAM DENSITY
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_channel = data["root.u_channel.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram_density(axis=ax7,
                            signal=o_channel.real,
                            samples_per_symbol=4, 
                            span_symbols=2,
                            offset_samples=2,
                            interp_n = 1000,
                            bins=200,
                            cmap='hot')

tit = r"Channel Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax7.set_title(tit)
ax7.set_xlabel(xlab)
ax7.set_ylabel(ylab)
# ax7.set_ylim(ax3.get_ylim())
ax7.set_facecolor('k')

#######################################
# AGC
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_agc = data["root.u_receiver.u_agc.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram_density(axis=ax8,
                            signal=o_agc.real,
                            samples_per_symbol=4, 
                            span_symbols=2,
                            offset_samples=2,
                            interp_n = 1000,
                            bins=200,
                            cmap='hot')

tit = r"AGC Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax8.set_title(tit)
ax8.set_xlabel(xlab)
ax8.set_ylabel(ylab)
# ax7.set_ylim(ax3.get_ylim())
ax8.set_facecolor('k')

#######################################
# FSE (taps evolution)
#######################################

clk_os_time = data["root.clk_os"]['s']
o_taps = data["root.u_receiver.u_lms.o_coeffs"]['s']

tit = r"FSE taps (real)"
xlab = r"Time [nsec]"
ylab = r"Amplitude"

ax9.plot(clk_os_time / 1e-9, o_taps.real)

ax9.set_title(tit)
ax9.set_xlabel(xlab)
ax9.set_ylabel(ylab)

#######################################
# FSE (mse)
#######################################

clk_time = data["root.clk"]['s']
o_slicer_error = data["root.u_receiver.u_slicer.o_error"]['s']

mse = np.square(np.abs(o_slicer_error))
mse = dsp.moving_average(mse, 1000)
mse = 10 * np.log10(mse)

tit = r"Mean Squared Error"
xlab = r"Time [nsec]"
ylab = r"MSE [dB]"

ax10.plot(clk_time / 1e-9, mse, color='b')
ax10.set_title(tit)
ax10.set_xlabel(xlab)
ax10.set_ylabel(ylab)

#######################################
# FSE EYE DIAGRAM
#######################################

min_time = 10e-9
max_time = 20e-9

clk_os_time = data["root.clk_os"]['s']
clk_os_time, x0_os, x1_os = dsp.trimmer(clk_os_time, min_time, max_time)

o_fse = data["root.u_receiver.u_fse.o_signal"]['s'][x0_os : x1_os]

plotter.eye_diagram_density(axis=ax11,
                            signal=o_fse.real,
                            samples_per_symbol=4, 
                            span_symbols=2,
                            offset_samples=3,
                            interp_n = 1000,
                            bins=200,
                            cmap='hot')

tit = r"FSE Output (real)"
xlab = r"Samples"
ylab = r"Amplitude"

ax11.set_title(tit)
ax11.set_xlabel(xlab)
ax11.set_ylabel(ylab)
ax11.set_facecolor('k')

#######################################
# DOWNSAMPLER DENSITY
#######################################

o_downsampler = data["root.u_receiver.u_downsampler.o_signal"]['s']

plotter.scatter_density(axis=ax12,
                        signal=o_downsampler,
                        bins=100,
                        cmap='hot')

tit = r"Downsampler Output"
xlab = r"In-phase"
ylab = r"Quadrature"

ax12.set_title(tit)
ax12.set_xlabel(xlab)
ax12.set_ylabel(ylab)
ax12.set_facecolor('k')

#######################################
# SLICER (evolution)
#######################################

clk_time = data["root.clk"]['s']
o_slicer = data["root.u_receiver.u_slicer.o_signal"]['s']
i_slicer = data["root.u_receiver.u_slicer.i_signal"]['s']

n_delay = 1
i_slicer = np.array(i_slicer)
i_slicer = np.roll(i_slicer, n_delay)
i_slicer[:n_delay] = np.NaN

ax13.scatter(clk_time / 1e-9, i_slicer.real, color='r', s=50)
ax13.scatter(clk_time / 1e-9, o_slicer.real, color='b', marker='x', linewidths=1, s=20)

tit = r"Slicer Input vs Output (real)"
xlab = r"Time [nsec]"
ylab = r"Amplitude"

ax13.set_title(tit)
ax13.set_xlabel(xlab)
ax13.set_ylabel(ylab)
ax13.set_ylim([x * 1.7 for x in ax13.get_ylim()])

#######################################
# SLICER
#######################################

min_time = 100e-9
max_time = 100.15e-9

clk_time = data["root.clk"]['s']
clk_time, x0, x1 = dsp.trimmer(clk_time, min_time, max_time)

o_slicer = data["root.u_receiver.u_slicer.o_signal"]['s']
i_slicer = data["root.u_receiver.u_slicer.i_signal"]['s']
o_slicer_error = data["root.u_receiver.u_slicer.o_error"]['s']
o_symb_ref = data["root.u_transmitter.u_symbol_gen.o_symb_ref"]['s']

n_delay = 10
o_symb_ref = np.array(o_symb_ref)
o_symb_ref = np.roll(o_symb_ref, n_delay)
o_symb_ref[:n_delay] = np.NaN

n_delay = 1
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

ax14.scatter(clk_time / 1e-9, i_slicer.real, color='r', s=100, label=label_in)
ax14.scatter(clk_time / 1e-9, o_slicer.real, color='b', marker='x',linewidths=4, label=label_out)
ax14.scatter(clk_time / 1e-9, o_symb_ref.real, 
            color='k',
            marker='s',
            edgecolors='black',
            facecolors='none',
            linewidths=2,
            s=200,
            label=label_ref)

ax14.scatter(clk_time / 1e-9, o_slicer_error.real, color='k', s=100, label=label_err)

tit = r"Slicer Comparison (real)"
xlab = r"Time [nsec]"
ylab = r"Amplitude"

ax14.set_title(tit)
ax14.set_xlabel(xlab)
ax14.set_ylabel(ylab)
ax14.set_ylim([x * 1.7 for x in ax14.get_ylim()])
ax14.legend(loc='upper center', ncol=4)

#######################################
# SAVING PLOTS
#######################################

plt.savefig('plots.png')