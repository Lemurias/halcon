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
# Date: 06/11/2024
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

import os
import re
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

results = halcon.load_signals("../run/logs/")

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

fig = plt.figure(figsize=(26, 16))
gs = GridSpec(2, 2)

ax1 = fig.add_subplot(gs[0,  0])
ax2 = fig.add_subplot(gs[0,  1])
ax3 = fig.add_subplot(gs[1,  0])
ax4 = fig.add_subplot(gs[1,  1])

#######################################
# Filter Input
#######################################

delay = 0
ticks = 100
x0 = delay
x1 = ticks + delay

tit = r"Filter Input"
xlab = r"Time [sec]"
ylab = r"Amplitude [V]"

y_v = results["root.u_filter_iir.i_signal"]['s'][x0 : x1]
x_v = results["root.clk"]['s'][x0 : x1]

ax1.stem(x_v, np.real(y_v), label='root.u_filter_iir.i_signal')

ax1.set_title(tit)
ax1.set_xlabel(xlab)
ax1.set_ylabel(ylab)
# ax1.legend(loc='upper right')


#######################################
# Filter Output
#######################################

tit = r"Filter Output"
xlab = r"Time [sec]"
ylab = r"Amplitude [V]"

y_v = results["root.u_filter_iir.o_signal"]['s'][x0 : x1]
x_v = results["root.clk"]['s'][x0 : x1]

ax2.plot(x_v, np.real(y_v), 'g', label='u_filter_iir.o_signal')

ax2.set_title(tit)
ax2.set_xlabel(xlab)
ax2.set_ylabel(ylab)
# ax2.legend(loc='upper right')

# ############################################################################################
# # 
# #######################################

y_v = results["root.u_filter_iir.i_signal"]['s']

# Fast Fourier Transform
signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal))
n = (signal.size)
fs = results["root.clk.frequency_hz"]['s']
timestep = 1/fs
freq = np.fft.fftfreq(n, d=timestep)

freq_pxx, pxx = sp.signal.welch(signal, fs=fs, nperseg=1024, noverlap=0, nfft=None, detrend='constant', return_onesided=False, scaling='density', axis=-1, average='mean')

# Convertion to dBm
mag_H_pxx = np.abs(pxx)/(np.max(pxx))
# Avoid zeros to avoid division by zero
mag_H_pxx[mag_H_pxx == 0] = 1e-15
mag_H_dbm_pxx = 10 * np.log10(mag_H_pxx)

ax3.plot(freq_pxx, mag_H_dbm_pxx, label='Filter Input PSD')

# #######################################
# # 
# #######################################


y_v = results["root.u_filter_iir.o_signal"]['s']

# Power Spectral Density
signal = np.array(y_v)
freq_pxx, pxx = sp.signal.welch(signal, fs=fs, nperseg=4096, noverlap=0, nfft=None, detrend='constant', return_onesided=False, scaling='density', axis=-1, average='mean')

# Convertion to dBm
mag_H_pxx = np.abs(pxx)/(np.max(pxx))
# Avoid zeros to avoid division by zero
mag_H_pxx[mag_H_pxx == 0] = 1e-15
mag_H_dbm_pxx = 10 * np.log10(mag_H_pxx)

ax3.plot(freq_pxx, mag_H_dbm_pxx, 'g', label='Filter Output PSD')

# #######################################
# # 
# #######################################

tit = r"Filter Response & Power Spectral Density"
xlab = r"Frequency [Hz]"
ylab = r"Power [dBm]"
label = r"Filter Input"

alpha = results["root.u_filter_iir.alpha"]['s']

# IIR Filter Coeffs
b = [alpha]  # num coeffs
a = [1.0000, -(1-alpha)]  # den coeffs

# Sample frequency
fs = results["root.clk.frequency_hz"]['s']

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
ax3.legend(loc='upper right')


#######################################################
# 
#######################################


tit = r"Impulse Response - Filter"
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

#######################################
# SAVING PLOTS
#######################################

plt.savefig('plots.png')