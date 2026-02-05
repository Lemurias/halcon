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

import matplotlib.pyplot as plt

def set_default_settings(font_size=25):

    # Layout
    plt.rcParams['figure.figsize'] = (12, 10)
    plt.rcParams['figure.autolayout'] = True
    
    # Font
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.size'] = font_size
    
    # Legend
    plt.rcParams['legend.fontsize'] = font_size - 2
    plt.rcParams['legend.loc'] = 'upper right'
    plt.rcParams['legend.shadow'] = True
    plt.rcParams['legend.framealpha'] = 1.0
    
    # Lines
    plt.rcParams['lines.linewidth'] = 2
    
    # Axis
    plt.rcParams['axes.labelsize'] = font_size - 1
    plt.rcParams['axes.labelpad'] = 5
    
    plt.rcParams['axes.xmargin'] = 0.03
    plt.rcParams['axes.ymargin'] = 0.03
    
    plt.rcParams['axes.titlepad'] = 10
    plt.rcParams['axes.titlelocation'] = 'center'
    plt.rcParams['axes.grid'] = True
    plt.rcParams['axes.titlesize'] = font_size

    # Ticks
    plt.rcParams['xtick.labelsize'] = font_size - 4
    plt.rcParams['ytick.labelsize'] = font_size - 4

    # Grid
    plt.rcParams['grid.color'] = 'gray'
    plt.rcParams['grid.linestyle'] = '--'
    plt.rcParams['grid.linewidth'] = 0.5
    plt.rcParams['grid.alpha'] = 0.8
    
    # Subplots
    plt.rcParams['figure.subplot.bottom'] = 0.1
    plt.rcParams['figure.subplot.top'] = 0.9
    plt.rcParams['figure.subplot.left'] = 0.1
    plt.rcParams['figure.subplot.right'] = 0.9
    plt.rcParams['figure.subplot.wspace'] = 0.5
    plt.rcParams['figure.subplot.hspace'] = 0.5
    
    # Export
    plt.rcParams['savefig.format'] = 'svg'