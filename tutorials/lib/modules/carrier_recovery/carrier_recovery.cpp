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
* Author: Mauro Nahuel Venancio
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

#include "carrier_recovery.hpp"

CarrierRecovery::CarrierRecovery()
{
    /* Registers */
    REFLECT(r_vco);
    REFLECT(r_int);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(i_symbol);
    REFLECT(o_signal);


    /* Variables */
    REFLECT(int_error);
    REFLECT(prop_error);

    /* Settings YAML */
    REFLECT_YAML(enable);
    REFLECT_YAML(k_p);
    REFLECT_YAML(k_i);
    REFLECT_YAML(k_vco);
    
}

void CarrierRecovery::Init()
{
    /* Pass */
}

void CarrierRecovery::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_vco);
    i_clock->RegisterOnPositiveEdge(this, r_int);

    /* Outputs */
    o_signal << p_hat << COMBINATIONAL_PORT;
}

void CarrierRecovery::RunClockMaster()
{
    if (enable)
    {
        /* Loop Filter */
        phase_error = asin(imag(i_signal.GetData() * conj(i_symbol.GetData())) / abs(i_symbol.GetData()) / abs(i_signal.GetData()));
        prop_error = phase_error * k_p;
        int_error = r_int.o * k_i;

        r_int.i = r_int.o + phase_error;

        /* VCO */
        r_vco.i = r_vco.o + int_error + prop_error;
        p_hat = exp(std::complex<double>(0, -1) * r_vco.o * k_vco);
    }
    else
    {
        p_hat = 1;
    }
}