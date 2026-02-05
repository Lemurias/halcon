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

#include "lagrange_interpolator.hpp"

LagrangeInterpolator::LagrangeInterpolator()
{
    /* Registers */
    REFLECT(r_shift_reg);
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_clock_rx);
    REFLECT(i_clock_ch);
    REFLECT(i_signal);
    REFLECT(o_signal);
}

void LagrangeInterpolator::Init()
{
    /* pass */
}

void LagrangeInterpolator::Connect()
{
    /* Register on clock positive edge */
    i_clock_ch->RegisterOnPositiveEdge(this, r_shift_reg);
    i_clock_rx->RegisterOnPositiveEdge(this, r_out);

    /* Outputs */
    o_signal << r_out.o;
}

void LagrangeInterpolator::RunClockMaster()
{
    for (size_t i = 3; i > 0; i--)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();
    
    double offset = fmod(i_clock_rx->GetTickCount() * i_clock_ch->GetFrequency() / i_clock_rx->GetFrequency(), 1.0);

    sum = r_shift_reg.o[3] * (-pow(offset, 3) / 6 + offset / 6) + 
          r_shift_reg.o[2] * ( pow(offset, 3) / 2 + pow(offset, 2) / 2 - offset) +
          r_shift_reg.o[1] * (-pow(offset, 3) / 2 - pow(offset, 2) + offset / 2 + 1) +
          r_shift_reg.o[0] * ( pow(offset, 3) / 6 + pow(offset, 2) / 2 + offset / 3);
    
    r_out.i = sum;
}