/*******************************************************************************
* ██████████████████████████████████████████████████████████████████████████████
* █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
* █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
* █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
* █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
* █    ███    ██▌          ██▌   ███████▄   ▀██▀   ▐██    ███    ███   ██▄     █
* █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
* ██████████████████████████████████████████████████████████████████████████████
* █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
* ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
********************************************************************************
* Author: Manuel Exequiel Arias
* Date: 05/16/2024
********************************************************************************
* MIT License
* 
* Copyright (c) 2024 Fundacion Fulgor
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
********************************************************************************/

#pragma once

/*******************************************************************************
* HEADERS
********************************************************************************/

#include <array>
#include "halcon.hpp"

/*******************************************************************************
* MODULE
********************************************************************************/

template<typename T, size_t N>
class ShiftRegister : public Module
{
private:

    /* Registers */
    Register<T, N> r_array { 0 };

public:

    ShiftRegister();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<double> i_signal;
    Output<T> o_signal;
    Output<T, N> o_shift_reg;
};

/*******************************************************************************
* CONSTRUCTOR (REFLECT MODULES, CLOCKS AND SETTINGS)
********************************************************************************/

template<typename T, size_t N>
ShiftRegister<T, N>::ShiftRegister()
{
    /* Registers */
    REFLECT(r_array);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);
    REFLECT(o_shift_reg);
}

/*******************************************************************************
* CONNECTIONS
********************************************************************************/

template<typename T, size_t N>
void ShiftRegister<T, N>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_array);
    i_clock->RegisterOnPositiveEdge(this, r_array);

    /* Output Port */
    o_signal << r_array.o[N - 1];

    for(size_t i { 0 }; i < N; ++i)
    {
        o_shift_reg[i] << r_array.o[i];
    }
}

/*******************************************************************************
* INITIALIZATION
********************************************************************************/

template<typename T, size_t N>
void ShiftRegister<T, N>::Init()
{
    /* Pass */
}

/*******************************************************************************
* BEHAVIOR
********************************************************************************/

template<typename T, size_t N>
void ShiftRegister<T, N>::RunClockMaster()
{
    /* Shift register */
    for(size_t i { 1 }; i < N; ++i)
    {
        r_array.i[i] = r_array.o[i - 1];
    }
    r_array.i[0] = static_cast<T>(i_signal.GetData());
}