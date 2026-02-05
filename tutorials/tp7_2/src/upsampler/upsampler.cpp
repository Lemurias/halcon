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
* Author: Manuel Exequiel Arias
* Date: 06/03/2024
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

#include "upsampler.hpp"

Upsampler::Upsampler()
{
    /* Registers */
    REFLECT(r_out_i);
    REFLECT(r_out_q);
    REFLECT(r_counter);

    /* Nodes */
    REFLECT(i_clock);
    REFLECT(i_n_ovr);
    REFLECT(i_signal);
    REFLECT(o_signal_i);
    REFLECT(o_signal_q);

    /* Variables */
    REFLECT(n_ovr);

    /* Settings YAML */
    REFLECT_YAML(phase);
}

void Upsampler::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_out_i);
    i_clock->RegisterOnPositiveEdge(this, r_out_q);
    i_clock->RegisterOnPositiveEdge(this, r_counter);

    /* Outputs */
    o_signal_i << r_out_i.o;
    o_signal_q << r_out_q.o;
}

void Upsampler::Init()
{
    /* Registers */
    r_counter.i = phase;

    /* Variables */
    i_n_ovr >> n_ovr;
}

void Upsampler::RunClockMaster()
{
    if ((r_counter.o + 1u) == n_ovr)
    {
        r_counter.i = 0u;
    }
    else
    {
        r_counter.i = r_counter.o + 1u;
    }

    if (r_counter.o == 0u)
    {
        r_out_i.i = i_signal.GetData().real();
        r_out_q.i = i_signal.GetData().imag();
    }
    else
    {
        r_out_i.i = 0;
        r_out_q.i = 0;
    }
}