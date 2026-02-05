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
* Date: 06/20/2024
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

#include "timing_recovery.hpp"

TimingRecovery::TimingRecovery()
{
    /* Registers */
    REFLECT(r_int);
    REFLECT(r_offset);
    REFLECT(r_base_pointer);

    /* Modules */
    REFLECT(u_upsampler);

    /* Ports */
    REFLECT(i_clock);
    REFLECT(i_clock_os);
    REFLECT(i_signal);
    REFLECT(o_offset);
    REFLECT(o_base_pointer);

    /* Variables */
    REFLECT(prop_error);
    REFLECT(int_error);

    /* Settings YAML */
    REFLECT_YAML(enable);
    REFLECT_YAML(kp);
    REFLECT_YAML(ki);
}

void TimingRecovery::Init()
{
    /* pass */
}

void TimingRecovery::Connect()
{
    i_clock_os->RegisterOnPositiveEdge(this, r_offset);
    i_clock_os->RegisterOnPositiveEdge(this, r_int);
    i_clock_os->RegisterOnPositiveEdge(this, r_base_pointer);
    i_clock_os->RegisterOnPositiveEdge(this, r_signal);

    /* Modules */
    u_upsampler.i_clock << i_clock_os;
    u_upsampler.i_signal << timing_error << COMBINATIONAL_PORT;
    u_upsampler.i_n_ovr << i_n_ovr;

    /* Outputs */
    o_offset << r_offset.o;
    o_base_pointer << r_base_pointer.o;
}

void TimingRecovery::RunClockMaster()
{
    // TED
    for (size_t i = NOS - 1; i > 0; i--)
    {
        r_signal.i[i] = r_signal.o[i - 1];
    }
    r_signal.i[0] = i_signal.GetData();

    timing_error = std::complex<double>(r_signal.o[1].real() * (r_signal.o[3].real() - i_signal.GetData().real()) +
                                        r_signal.o[1].imag() * (r_signal.o[3].imag() - i_signal.GetData().imag()), 0);

    // Timing Recovery
    if (enable)
    {
        /* Loop Filter */
        r_int.i = r_int.o + u_upsampler.o_signal.GetData().real() * ki;
        prop_error = u_upsampler.o_signal.GetData().real() * kp;
        int_error = r_int.o;
        total_error = prop_error + int_error;

        r_offset.i = r_offset.o + total_error;
        if (r_offset.i >= 1)
        {
            r_offset.i = r_offset.i - 1;
            r_base_pointer.i = r_base_pointer.o + 1;
        }
        if (r_offset.i < 0)
        {
            r_offset.i = r_offset.i + 1;
            r_base_pointer.i = r_base_pointer.o - 1;
        }
    }
}