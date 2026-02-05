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
# Date: 05/17/2024
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

fig = plt.figure(figsize=(16, 8))
gs = GridSpec(1, 1)

ax1 = fig.add_subplot(gs[0, 0])

#######################################
# SYMBOL GENERATOR
#######################################

delay = 0
ticks = 101
phase = 0
x0 = phase + delay
x1 = ticks + phase + delay

#######################################
# SHIFT REGISTER OUTPUT
#######################################

y_v = data["root.u_sin_gen.o_sin"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]

tit = "Shift Register Input & Output"
fname = tit.replace(" ", "_").lower()
xlab = "Tiempo"
ylab =  "Amplitud"

ax1.plot(x_v, y_v, '.r')
ax1.set_title(tit)
ax1.set_xlabel(xlab)
ax1.set_ylabel(ylab)

#######################################
#       SHIFT REGISTER OUTPUT
#######################################

y_v = data["root.u_shift_reg.o_signal"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]

ax1.plot(x_v, y_v, '.b')
ax1.set_title(tit)
ax1.set_xlabel(xlab)
ax1.set_ylabel(ylab)

#######################################
# SAVING PLOTS
#######################################

plt.savefig('plots.png')