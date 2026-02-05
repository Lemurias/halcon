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
class AGC : public Module
{
private:

    /* Registers */
    Register<std::complex<double>> r_out;
    Register<double, N - 1> r_shift_reg;
    Register<double> r_int;

    /* Settings YAML */
    double step;
    double reference_power;

public:

    AGC();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<std::complex<double>> i_signal;
    Output<std::complex<double>> o_signal;
};

template <size_t N>
AGC<N>::AGC()
{
    /* Registers */
    REFLECT(r_out);
    REFLECT(r_int);
    REFLECT(r_shift_reg);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);

    /* Settings YAML */
    REFLECT_YAML(reference_power);
    REFLECT_YAML(step);
}

template <size_t N>
void AGC<N>::Init()
{
    r_int.Set();
}

template <size_t N>
void AGC<N>::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_out);
    i_clock->RegisterOnPositiveEdge(this, r_int);
    i_clock->RegisterOnPositiveEdge(this, r_shift_reg);

    /* Outputs */
    o_signal << r_out.o;
}

template <size_t N>
void AGC<N>::RunClockMaster()
{
    r_out.i = r_int.o * i_signal.GetData();
    
    double signal_power = pow(r_out.i.real(), 2) + pow(r_out.i.imag(), 2);

    for (size_t i {N - 2}; i > 0; i--)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = signal_power;

    double average_power = signal_power / N;
    for (size_t i { 0 }; i < (N - 1); i++)
    {
        average_power += r_shift_reg.o[i] / N;
    }
    
    r_int.i = r_int.o + step * (reference_power - average_power);
}