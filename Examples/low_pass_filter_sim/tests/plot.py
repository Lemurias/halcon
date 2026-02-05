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
# Date: 07/31/2024
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
import scipy as sp
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

plotter.set_default_settings(20)

#######################################
# FIGURE SETTINGS
#######################################

fig = plt.figure(figsize=(20, 10))
gs = GridSpec(2, 3)

ax1 = fig.add_subplot(gs[0,  0])
ax2 = fig.add_subplot(gs[0,  1])
ax3 = fig.add_subplot(gs[0,  2])
ax4 = fig.add_subplot(gs[1,  0])
ax5 = fig.add_subplot(gs[1,  1])
ax6 = fig.add_subplot(gs[1,  2])

delay = 0
ticks = 10000
phase = 0
x0 = phase + delay
x1 = ticks + phase + delay

###############################################################
#    SIN GENERATOR OUTPUT - LOW & HIGH FRECUENCY (log node)
###############################################################

tit = "Sine Generator - Low & High Frecuency"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"

# Low Frecuency
y_v = data["root.u_sin_low.o_sin"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]
ax1.plot(x_v, y_v)

# High Frecuency
y_v = data["root.u_sin_high.o_sin"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]
ax1.plot(x_v, y_v)

ax1.grid(True)
ax1.set_title(f'{tit}')
ax1.set_xlabel(xlab)
ax1.set_ylabel(ylab)
fs = data["root.clk.i_frequency_hz"]['s']
ax1.set_xlim(10/fs,(100+1)/fs)


###############################################################
#    ADDER OUTPUT - LOW + HIGH FRECUENCY (log node)
###############################################################

tit = "Adder output - Low + High Frecuency"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"

# Adder output
y_v = data["root.u_adder.o_sum"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]
ax2.plot(x_v, y_v)

ax2.grid(True)
ax2.set_title(f'{tit}')
ax2.set_xlabel(xlab)
ax2.set_ylabel(ylab)
fs = data["root.clk.i_frequency_hz"]['s']
ax2.set_xlim(10/fs,(100+1)/fs)


###############################################################
#    FILTER OUTPUT (log node)
###############################################################

tit = "Filter output"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"

# Filter output
y_v = data["root.u_filter.o_signal"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]
ax3.plot(x_v, y_v)

ax3.grid(True)
ax3.set_title(f'{tit}')
ax3.set_xlabel(xlab)
ax3.set_ylabel(ylab)
fs = data["root.clk.i_frequency_hz"]['s']
ax3.set_xlim(10/fs,(100+1)/fs)

###############################################################
#    FOURIER TRANSFORM - LOW + HIGH FRECUENCY (log node)
###############################################################

tit = "Fast Fourier Transform - Low + High Frecuency"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Amplitude [dBm]"

#################
## Low Frecuency
y_v = data["root.u_adder.o_sum"]['s'][x0 : x1]
x_v = data["root.clk"]['s'][x0 : x1]

# Fast Fourier Transform
signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal)*8)
n = 8*(signal.size)
fs = data["root.clk.i_frequency_hz"]['s']
timestep = 1/fs
freq = np.fft.fftfreq(n, d=timestep)

# Convertion to dBm
mag_H = np.abs(fourier)
mag_H[mag_H == 0] = 1e-15  # Avoid zeros to avoid division by zero
mag_H_dbm = 10 * np.log10((mag_H/len(signal))/(1e-3))

# Plot
ax4.plot(freq, mag_H_dbm)

ax4.grid(True)
ax4.set_title(f'{tit}')
ax4.set_xlabel(xlab)
ax4.set_ylabel(ylab)
ax4.set_ylim((20,30))


###############################################################
#    FOURIER TRANSFORM - FILTER TAPS (log node)
###############################################################

# Low Frecuency
y_v = data["root.u_filter.coeffs"]['s']
x_v = data["root.clk"]['s']

tit = "Fourier Transform - Filter Taps"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Amplitude [dBm]"

signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal)*8)
n = 8*(signal.size)
timestep = 0.01
freq = np.fft.fftfreq(n, d=timestep)

# Número de puntos en la respuesta en frecuencia
N = 1000

H = np.fft.fft(y_v, N)
H_shifted = np.fft.fftshift(H)
w = np.linspace(-np.pi, np.pi, N)

# Evitar el error de división por cero al tomar el logaritmo
mag_H = np.abs(H_shifted)
mag_H[mag_H == 0] = 1e-15  # Evitar ceros para evitar divisiones por cero
mag_H_dbm = 10 * np.log10((mag_H/len(signal))/(1e-3))

fs = data["root.clk.i_frequency_hz"]['s']
frequencies_hz = w * (fs / (2 * np.pi))

# Graficar la respuesta en frecuencia
ax5.plot(frequencies_hz, mag_H_dbm, 'b')

# plt.plot(freq, abs(fourier)/(len(signal)))
ax5.grid(True)
ax5.set_title(f'{tit}')
ax5.set_xlabel(xlab)
ax5.set_ylabel(ylab)
ax5.set_ylim((0,50))

###########################################################
#    FOURIER TRANSFORM - FILTER OUTPUT SIGNAL (log node)
###########################################################

y_v = data["root.u_filter.o_signal"]['s']
x_v = data["root.clk"]['s']

tit = "Fourier Transform - Filter - o_signal"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Amplitude [dBm]"

signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal)*8)
n = 8*(signal.size)
fs = data["root.clk.i_frequency_hz"]['s']
timestep = 1/fs
freq = np.fft.fftfreq(n, d=timestep)


# Convertion to dBm
mag_H = np.abs(fourier)
mag_H[mag_H == 0] = 1e-15  # Avoid zeros to avoid division by zero
mag_H_dbm = 10 * np.log10((mag_H/len(signal))/(1e-3))

# Plot
ax6.plot(freq, mag_H_dbm)

ax6.grid(True)
ax6.set_title(f'{tit}')
ax6.set_xlabel(xlab)
ax6.set_ylabel(ylab)
ax6.set_ylim((20,40))


plt.savefig('plots.png')

plt.show()