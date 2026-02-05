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

###############################################################################
#                           POST SIMULATION PLOTS
###############################################################################

import os
import numpy as np
import matplotlib.pyplot as plt

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

###############################################################################
#                                   PLOTS
###############################################################################

#######################################
# MATPLOTLIB GLOBAL SETTINGS
#######################################

plt.rcParams['figure.figsize'] = (30, 14)                       # Tamaño de la figura
plt.rcParams['figure.autolayout'] = True                        # Ajuste automático del espacio alrededor de la figura
plt.rcParams['font.family'] = 'serif'                           # Familia de fuentes: serif
plt.rcParams['font.size'] = 22                                  # Tamaño de la fuente global

# plt.rcParams['legend.fontsize'] = 'medium'                      # Tamaño de fuente de la leyenda
plt.rcParams['legend.loc'] = 'upper right'                      # Posición por defecto de la leyenda
plt.rcParams['legend.shadow'] = True                            # Habilita el efecto de sombreado en la leyenda
plt.rcParams['legend.framealpha'] = 1.0                         # Establece el valor alfa del marco de la leyenda a 1.0 (sin transparencia)

plt.rcParams['lines.linewidth'] = 2                             # Ancho de linea

# plt.rcParams['axes.labelsize'] = 'medium'                       # Tamaño de fuente de las etiquetas de los ejes
plt.rcParams['axes.labelpad'] = 5                               # Espacio entre la etiqueta y el eje
plt.rcParams['axes.xmargin'] = 0.03                             # Margen extra en el eje X
plt.rcParams['axes.ymargin'] = 0.03                             # Margen extra en el eje Y
plt.rcParams['axes.titlepad'] = 10                              # Espacio entre el título y el gráfico
plt.rcParams['axes.titlesize'] = 20                             # Tamaño de la fuente del título
plt.rcParams['axes.titlelocation'] = 'center'                   # Ubicación del título

plt.rcParams['xtick.labelsize'] = 'small'                      # Tamaño de fuente de las etiquetas del eje X
# plt.rcParams['ytick.labelsize'] = 'medium'                      # Tamaño de fuente de las etiquetas del eje Y

plt.rcParams['axes.grid'] = True                                # Activa la grilla por defecto
plt.rcParams['grid.color'] = 'gray'                             # Color de la grilla
plt.rcParams['grid.linestyle'] = '--'                           # Estilo de línea de la grilla
plt.rcParams['grid.linewidth'] = 0.5                            # Ancho de línea de la grilla
plt.rcParams['grid.alpha'] = 0.8                                # Transparencia de la grilla

plt.rcParams['figure.subplot.bottom'] = 0.1                     # Espacio entre la parte inferior del subplot y el borde de la figura
plt.rcParams['figure.subplot.top'] = 0.9                        # Espacio entre la parte superior del subplot y el borde de la figura
plt.rcParams['figure.subplot.left'] = 0.1                       # Espacio entre la parte izquierda del subplot y el borde de la figura
plt.rcParams['figure.subplot.right'] = 0.9                      # Espacio entre la parte derecha del subplot y el borde de la figura
plt.rcParams['figure.subplot.wspace'] = 0.5                     # Espacio horizontal entre subplots
plt.rcParams['figure.subplot.hspace'] = 0.5                     # Espacio vertical entre subplots

plt.rcParams['savefig.format'] = 'svg'                          # Guardar gráficos en formato SVG



delay = 0
ticks = 30001
phase = 0
x0 = phase + delay
x1 = ticks + phase + delay

plt.figure(figsize=(16,20))

###############################################################
#    SIN GENERATOR OUTPUT (log node)
###############################################################

tit = "Sine Generator Output"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"
plt.subplot(3,2,1)

# Low Frecuency
y_v = results["root.u_sin_gen.o_sin"][x0 : x1]
x_v = results["root.clk"][x0 : x1]
plt.plot(x_v, y_v)

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)
plt.xlim(0,0.0001)


###############################################################
#    FILTER OUTPUT (log node)
###############################################################

tit = "Filter Output"
fname = tit.replace(" ", "_").lower()
xlab = "Time [sec]"
ylab = "Amplitude [V]"
plt.subplot(3,2,2)

# Filter output
y_v = results["root.u_filter.o_signal"][x0 : x1]
x_v = results["root.clk"][x0 : x1]
plt.plot(x_v, y_v)

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)
plt.xlim(0,0.0001)


###############################################################
#    FOURIER TRANSFORM - SINE GENERATOR (log node)
###############################################################

tit = "FFT - Sine Generator Output"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Amplitude [dBm]"
plt.subplot(3,2,3)

#################
## Low Frecuency
y_v = results["root.u_sin_gen.o_sin"][x0 : x1]
x_v = results["root.clk"][x0 : x1]

# Fast Fourier Transform
signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal)*8)
n = 8*(signal.size)
timestep = 1/results["root.clk.frequency_hz"]
freq = np.fft.fftfreq(n, d=timestep)

# Convertion to dBm
mag_H = np.abs(fourier)
mag_H[mag_H == 0] = 1e-15  # Avoid zeros to avoid division by zero
mag_H_dbm = 10 * np.log10((mag_H/len(signal))/(1e-3))

# Plot
plt.plot(freq, mag_H_dbm)
# plt.plot(freq, mag_H_db/(len(signal)))
#################

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)
plt.ylim((0,50))


###########################################################
#    FOURIER TRANSFORM - FILTER OUTPUT SIGNAL (log node)
###########################################################

y_v = results["root.u_filter.o_signal"]
x_v = results["root.clk"]

tit = "FFT - Filter Output"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Amplitude [dBm]"
plt.subplot(3,2,4)

signal = np.array(y_v)
fourier = np.fft.fft(signal, n=len(signal)*8)
n = 8*(signal.size)
timestep = 1/results["root.clk.frequency_hz"]
freq = np.fft.fftfreq(n, d=timestep)


# Convertion to dBm
mag_H = np.abs(fourier)
mag_H[mag_H == 0] = 1e-15  # Avoid zeros to avoid division by zero
mag_H_dbm = 10 * np.log10((mag_H/len(signal))/(1e-3))

# Plot
plt.plot(freq, mag_H_dbm)
# plt.plot(freq, abs(mag_H_db)/(len(signal)))
#################

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)
plt.ylim((0,50))


###############################################################
#    FOURIER TRANSFORM - FILTER TAPS (log node)
###############################################################
plt.rcParams['xtick.labelsize'] = 'medium'                      # Tamaño de fuente de las etiquetas del eje X

# Low Frecuency
y_v = results["root.u_filter.coeffs"]
x_v = results["root.clk"]

tit = "Fourier Transform - Filter Taps"
fname = tit.replace(" ", "_").lower()
xlab = "Frecuency [Hz]"
ylab = "Amplitude [dB]"
plt.subplot(3,2,(5,6))

# Number of points in the frequency response
N = 1000

H = np.fft.fft(y_v, N)
H_shifted = np.fft.fftshift(H)
w = np.linspace(-np.pi, np.pi, N)

## Division by zero
mag_H = np.abs(H_shifted)
# Avoid zeros to avoid division by zero
mag_H[mag_H == 0] = 1e-15
mag_H_dbm = 10 * np.log10((mag_H))

fs = results["root.clk.frequency_hz"]
frequencies_hz = w * (fs / (2 * np.pi))

plt.plot(frequencies_hz, mag_H_dbm, 'b')

plt.grid(True)
plt.title(f'{tit}')
plt.xlabel(xlab)
plt.ylabel(ylab)
plt.ylim((-20,10))


plt.savefig('plots.png')