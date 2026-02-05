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
# Author: Patricio Reus Merlo
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

################################################################################
# PYTHON MODULES
################################################################################

import numpy as np

################################################################################
# PARAMETER CLASS
################################################################################

class Parameter:

    __list = []

    ############################################################################
    # CONSTRUCTOR
    ############################################################################

    def __init__(self, name, text, alpha, value, simh=None, list=True, index=0):
        
        if simh != None:
            simh.check_if_exists_in_hierarchy(name)
        
        self.__name = name
        self.__text = text
        self.__alpha = alpha
        self.__index = index
        self.__value = np.array(value).tolist()

        if list:
            Parameter.__list.append(self)

    ############################################################################3
    # PRIVATE METHODS
    ############################################################################

    def __len__(self):
        return len(self.__value)
    
    def __index__(self):
        return len(self)

    def __iter__(self):
        for i in range(self):
            yield i, self.__getitem__(i)

    def __iter__(self):
        for i in range(self):
            yield self.__getitem__(i)

    def __getitem__(self, index):
        return Parameter (
            name = self.__name,
            text = self.__text,
            alpha = self.__alpha,
            value = self.__value[index],
            index = index,
            simh = None, 
            list = False
        )
    
    def __str__(self):
        return str(self.__value)
    
    ############################################################################
    # PUBLIC  METHODS
    ############################################################################

    @property
    def value(self):
        return self.__value
    
    @property
    def index(self):
        return self.__index
    
    @property
    def name(self):
        return self.__name

    @property
    def description(self):
        return self.__text.format(self.__value * self.__alpha)
    
    ############################################################################
    # CLASS METHODS
    ############################################################################

    @classmethod
    def list(cls):
        return cls.__list