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
# Author: Patricio Reus Merlo
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

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

def eye_diagram_density(axis, signal, samples_per_symbol, 
                        span_symbols=2, offset_samples=0, interp_n= 1000,
                        bins=200, cmap='hot'):

    signal = np.array(signal)   
    num_segments = (len(signal) - offset_samples) // samples_per_symbol - span_symbols + 1

    x_axis = np.linspace(- (span_symbols / 2) * samples_per_symbol, 
                         + (span_symbols / 2) * samples_per_symbol, 
                         span_symbols * samples_per_symbol + 1)
    
    x_interp = np.linspace(x_axis[0], x_axis[-1], interp_n)

    eye_data = []

    for i in range(num_segments):
        start_idx = i * samples_per_symbol + offset_samples
        end_idx = start_idx + span_symbols * samples_per_symbol + 1
        
        if end_idx <= len(signal):
            y_axis = signal[start_idx : end_idx]
            interp_func = interp1d(x_axis, y_axis, kind='cubic')
            y_interp = interp_func(x_interp)
            eye_data.append(y_interp)

    eye_data = np.array(eye_data)

    axis.hist2d(np.tile(x_interp, len(eye_data)), np.ravel(eye_data), bins=(bins, bins), cmap=cmap)
    axis.set_xlim([np.min(x_interp), np.max(x_interp)])