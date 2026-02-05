/*******************************************************************************
* ██████████████████████████████████████████████████████████████████████████████
* █▀▀▀▀███▀▀▀▀█████▀▀▀▀▀██████▀▀▀█████████▀▀▀▀▀▀▀███████▀▀▀▀▀▀▀█████▀▀▀▀████▀▀▀█
* █    ███    ████▌      ████▌   ████████    ▄▄    ███▀   ▄▄▄   ▀███     ▀██   █
* █    ▀▀▀    ████   ▄   ▐███▌   ███████    ████▄▄▄██▌   ▐███▌   ▐██       ▀   █
* █           ███   ▐█▌   ███▌   ███████    █████████▌   ▐███▌    ██   ▄       █
* █    ███    ██▌          ██▌   ███████▄   ▀██▀   ███    ███    ███   ██▄     █
* █    ███    ██   █████   ██▌        ████▄      ▄█████▄       ▄████   ████▄   █
* ██████████████████████████████████████████████████████████████████████████████
* █████████████████████████ DSP SIleakageLATION ENGINE ██████████████████████████████
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
#include <array>

#include "halcon.hpp"

template <size_t N>
class LMS : public Module
{
private:

    /* Registers */
    Register<std::complex<double>, (N - 1)> r_shift_reg { 0 };
    Register<std::complex<double>, N> r_out { 0 };

    /* Variables */
    std::array<std::complex<double>, N> gradient;

    /* Settings YAML */
    std::array<std::complex<double>, N> coeffs_init;
    double leakage;
    double step;

public:

    LMS();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;
    
    /* Ports */
    Input<Clock> i_clock;
    Input<std::complex<double>> i_signal;
    Input<std::complex<double>> i_error;
    Output<std::complex<double>, N> o_coeffs;
};

template <size_t N>
LMS<N>::LMS()
{
    /* Registers */
    REFLECT(r_out);
    REFLECT(r_shift_reg);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(i_error);
    REFLECT(o_coeffs);

    /* Variables */
    REFLECT(gradient);

    /* Settings YAML */
    REFLECT_YAML(leakage);
    REFLECT_YAML(step);
    REFLECT_YAML(coeffs_init);
}

template <size_t N>
void LMS<N>::Init()
{
    r_out.Set(coeffs_init);
}

template <size_t N>
void LMS<N>::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_out);
    i_clock->RegisterOnPositiveEdge(this, r_shift_reg);

    /* Outputs */
    for (size_t i = 0; i < N; i++)
    {
        o_coeffs[i] << r_out.o[i];
    }
}

template <size_t N>
void LMS<N>::RunClockMaster()
{
    for (size_t i = (N - 2); i > 0; i--)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();

    for (size_t i = 0; i < N; i++)
    {
        if (i == 0)
        {
            gradient[i] = i_error.GetData() * conj(i_signal.GetData());
        }
        else
        {
            gradient[i] = i_error.GetData() * conj(r_shift_reg.o[i - 1]);
        }

        r_out.i[i] = r_out.o[i] * (1 - leakage * step) + step * gradient[i];
    }
}