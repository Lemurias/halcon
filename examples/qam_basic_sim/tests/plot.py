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
# Author: Patricio Reus Merlo, Mauro Nahuel Venancio
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

import plotter
import halcon
import dsp

###############################################################################
# LOAD SIGNALS
###############################################################################

data = halcon.Processor.load_directory("../run/logs/")

###############################################################################
# PLOTS
###############################################################################

#######################################
# MATPLOTLIB GLOBAL SETTINGS
#######################################

plotter.set_default_settings(30)

#######################################
# FIGURE SETTINGS
#######################################

fig = plt.figure(figsize=(24, 8))
gs = GridSpec(1, 3)

ax1 = fig.add_subplot(gs[0, 0])
ax2 = fig.add_subplot(gs[0, 1])
ax3 = fig.add_subplot(gs[0, 2])

#######################################
# SYMBOL GENERATOR
#######################################

delay = 0
ticks = 1000
x0 = delay
x1 = ticks + delay

clk = data["root.clk"]['s'][x0 : x1]
o_symb_tx = data["root.u_transmitter.u_symbol_gen.o_symb_tx"]['s'][x0 : x1]

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
ax1.set_ylim([x * 1.4 for x in ax1.set_ylim()])
ax1.set_xlim([x * 1.2 for x in ax1.set_ylim()])

#######################################
# AWGN CHANNEL OUTPUT
#######################################

o_channel_awgn = data["root.u_channel.u_awgn_ch.o_signal"]['s']

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
ax2.set_ylim([x * 1.2 for x in ax2.set_ylim()])
ax2.set_xlim([x * 1.1 for x in ax2.set_ylim()])

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

ax3.scatter(clk_time / 1e-9, i_slicer.real, color='r', s=50)
ax3.scatter(clk_time / 1e-9, o_slicer.real, color='b', marker='x', linewidths=1, s=20)

tit = r"Slicer Input vs Output (real)"
xlab = r"Time [nsec]"
ylab = r"Amplitude"

ax3.set_title(tit)
ax3.set_xlabel(xlab)
ax3.set_ylabel(ylab)
ax3.set_ylim([x * 1.2 for x in ax3.get_ylim()])

#######################################
# SAVING PLOTS
#######################################

plt.savefig('plots.png')