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


#include "filter.hpp"

Filter::Filter()
{
    /* Registers */
    REFLECT(r_shift_reg);
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);
    REFLECT(o_shift_reg);

    /* Settings YAML */
    REFLECT_YAML(coeffs);
}

void Filter::Init()
{

}

void Filter::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_shift_reg);
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Output Port */
    o_signal << r_out.o;

    for(size_t i { 1 }; i < NTAPS; ++i)
    {
        o_shift_reg[i] << r_shift_reg.o[i - 1];
    }
    o_shift_reg[0] << i_signal;
}

void Filter::RunClockMaster()
{
    /* Shift register */
    for(size_t i { 1 }; i < NTAPS - 1; ++i)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();

    /* Products */
    sum = i_signal.GetData() * coeffs[0];
    for(size_t i { 1 }; i < NTAPS; ++i)
    {
        sum += r_shift_reg.o[i - 1] * coeffs[i];
    }
    r_out.i = sum;
}
