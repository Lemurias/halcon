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
#                           POST SIMULATION PLOTS
###############################################################################

###############################################################################
# PYTHON MODULES
###############################################################################

import os
import numpy as np
import scipy as sp
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec


###############################################################################
# HALCON MODULES
###############################################################################

import plotter
import dsp


###############################################################################
#                               LOAD SIGNALS
###############################################################################

signals_path = '../run/logs'
signals_list = os.listdir(signals_path)

results = {}
names = []

for signal_name in signals_list:
    name = signal_name.split('_CLOCK')[0].split('SIGNAL_')[1]
    names.append(name)
    print(f"Loading signal: {name} ...")

    if 'cpx' not in signal_name:
        try:
            signal = np.loadtxt(f'{signals_path}/{signal_name}')
        except Exception as e:
            print(e)
    else:
        try:
            signal = np.loadtxt(f'{signals_path}/{signal_name}', dtype=complex)
        except Exception as e:
            print(e)
    
    results[name] = signal



#######################################
# MATPLOTLIB GLOBAL SETTINGS
#######################################

plotter.set_default_settings(20)

###############################################################################
#                                   PLOTS
###############################################################################

delay = 0
ticks = 100
phase = 0
x0 = phase + delay
x1 = ticks + phase + delay

plt.figure(figsize=(20,14))


##############################################################################################################################
#                                                            SUBPLOT 1
##############################################################################################################################

###############################################################
#                  FILTER INPUT (TIME)
###############################################################

tit = "Filter Input"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"
plt.subplot(2,2,1)

# Filter output
y_v = results["root.u_upsampling.o_signal"][x0 : x1]
x_v = results["root.clk_oversampling"][x0 : x1]
plt.stem(x_v, np.real(y_v))

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)

##############################################################################################################################
#                                                            SUBPLOT 2
##############################################################################################################################

###############################################################
#                    FILTER OUTPUT (TIME) 
###############################################################

tit = "Filter Output"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"
plt.subplot(2,2,2)

# Filter output
offset = int(len(results["root.u_rrc_filter.coeffs"])/2)
offset = 0
y_v = results["root.u_rrc_filter.o_signal"][x0 : x1]
x_v = results["root.clk_oversampling"][x0 : x1]
plt.plot(x_v, np.real(y_v), 'g')

plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)

##############################################################################################################################
#                                                            SUBPLOT 3
##############################################################################################################################

###############################################################
#            FOURIER TRANSFORM - FILTER TAPS
###############################################################

y_v = results["root.u_rrc_filter.coeffs"]
x_v = results["root.clk_oversampling"]

tit = "RRC Filter Response & Power Spectral Density"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Power [dBm]"
plt.subplot(2,2,3)

# Number of response frecuency points
N = 1000

H = np.fft.fft(y_v, N)
H_shifted = np.fft.fftshift(H)
w = np.linspace(-np.pi, np.pi, N)

# Convertion to dBm
mag_H = np.abs(H_shifted)**2
mag_H = mag_H/(np.max(mag_H))
 # Avoid zeros to avoid division by zero
mag_H[mag_H == 0] = 1e-15
mag_H_dbm = 10 * np.log10(mag_H)
fs = results["root.clk_oversampling.frequency_hz"]
frequencies_hz = w * (fs / (2 * np.pi))

# Graficar la respuesta en frecuencia
plt.plot(frequencies_hz, mag_H_dbm, 'r', label='RRC Filter')


###############################################################
#             FOURIER TRANSFORM - FILTER INPUT
###############################################################

y_v = results["root.u_upsampling.o_signal"]
x_v = results["root.clk_oversampling"]

# Fast Fourier Transform
signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal))
n = (signal.size)
fs = results["root.clk_oversampling.frequency_hz"]
timestep = 1/fs
freq = np.fft.fftfreq(n, d=timestep)

freq_pxx, pxx = sp.signal.welch(signal, fs=fs, nperseg=1024, noverlap=0, nfft=None, detrend='constant', return_onesided=False, scaling='density', axis=-1, average='mean')

# Convertion to dBm
mag_H_pxx = np.abs(pxx)/(np.max(pxx))
# Avoid zeros to avoid division by zero
mag_H_pxx[mag_H_pxx == 0] = 1e-15
mag_H_dbm_pxx = 10 * np.log10(mag_H_pxx)

# Plot
plt.plot(freq_pxx, mag_H_dbm_pxx, label='Filter Input PSD')


###############################################################
#             FOURIER TRANSFORM - FILTER OUTPUT
###############################################################

y_v = results["root.u_rrc_filter.o_signal"]
x_v = results["root.clk_oversampling"]

# Power Spectral Density
signal = np.array(y_v)
freq_pxx, pxx = sp.signal.welch(signal, fs=fs, nperseg=1024, noverlap=0, nfft=None, detrend='constant', return_onesided=False, scaling='density', axis=-1, average='mean')

# Convertion to dBm
mag_H_pxx = np.abs(pxx)/(np.max(pxx))
# Avoid zeros to avoid division by zero
mag_H_pxx[mag_H_pxx == 0] = 1e-15
mag_H_dbm_pxx = 10 * np.log10(mag_H_pxx)

# Plot
plt.plot(freq_pxx, mag_H_dbm_pxx, 'g', label='Filter Output PSD')
# plt.rcParams['legend.fontsize'] = '16' 
plt.rcParams['legend.loc'] = 'lower right' 
plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)
plt.legend()


###############################################################
#             FOURIER TRANSFORM - FILTER TAPS
###############################################################

y_v = results["root.u_rrc_filter.coeffs"]
x_v = results["root.clk_oversampling"]

tit = "Impulse Response - Root-Raised-Cosine Filter"
fname = tit.replace(" ", "_").lower()
xlab = "Samples"
ylab = "Amplitude"
plt.subplot(2,2,4)

# Number of response frecuency points
N = 1000

H = np.fft.fft(y_v, N)
H_shifted = np.fft.fftshift(H)
w = np.linspace(-np.pi, np.pi, N)

## Convertion to dBm
mag_H = np.abs(H_shifted)
# Avoid zeros to avoid division by zero
mag_H[mag_H == 0] = 1e-15
mag_H_dbm = 10 * np.log10((mag_H/len(y_v))/(1e-3))

fs = results["root.clk_oversampling.frequency_hz"]
frequencies_hz = w * (fs / (2 * np.pi))

plt.plot(y_v, 'r', marker="s")

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)



plt.savefig('plots.png')