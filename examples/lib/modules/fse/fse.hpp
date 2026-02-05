/*******************************************************************************
* ██████████████████████████████████████████████████████████████████████████████
* █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
* █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
* █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
* █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
* █    ███    ██▌          ██▌   ███████▄   ▀██▀   ███    ███    ███   ██▄     █
* █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
* ██████████████████████████████████████████████████████████████████████████████
* █████████████████████████ DSP SIMULATION ENGINE ██████████████████████████████
* ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
********************************************************************************
* Author: Mauro Venancio
* Date: 05/17/2024
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

#include <complex>

#include "halcon.hpp"

template <size_t N>
class FractionallySpacedEqualizer : public Module
{
private:

    /* Registers */
    Register<std::complex<double>, (N - 1)> r_shift_reg;
    Register<std::complex<double>> r_out;

    /* Variables */
    std::complex<double> result;

public:

    FractionallySpacedEqualizer();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;
    
    /* Ports */
    Input<Clock> i_clock;
    Input<std::complex<double>> i_signal;
    Input<std::complex<double>, N> i_coeffs;
    Output<std::complex<double>> o_signal;
};

template <size_t N>
FractionallySpacedEqualizer<N>::FractionallySpacedEqualizer()
{
    /* Registers */
    REFLECT(r_out);
    REFLECT(r_shift_reg);

    /* Ports */
    REFLECT(i_clock);
    REFLECT(i_signal);
    REFLECT(i_coeffs);
    REFLECT(o_signal);
}

template <size_t N>
void FractionallySpacedEqualizer<N>::Init()
{
    /* pass */
}

template <size_t N>
void FractionallySpacedEqualizer<N>::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_out);
    i_clock->RegisterOnPositiveEdge(this, r_shift_reg);

    /* Outputs */
    o_signal << r_out.o;
}

template <size_t N>
void FractionallySpacedEqualizer<N>::RunClockMaster()
{
    for (size_t i = (N - 2); i > 0; i--)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();

    result = i_signal.GetData() * i_coeffs[0].GetData();
    for (size_t i = 0; i < (N - 1); i++)
    {
        result += r_shift_reg.o[i] * i_coeffs[i + 1].GetData();
    }

    r_out.i = result;
}