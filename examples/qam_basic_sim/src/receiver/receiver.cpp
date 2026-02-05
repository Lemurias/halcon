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
* Author: Mauro Nahuel Venancio
* Date: 05/28/2024
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

/*******************************************************************************
* HEADERS
********************************************************************************/

#include "receiver.hpp"

/*******************************************************************************
* CONSTRUCTOR (REFLECT MODULES, CLOCKS AND SETTINGS)
********************************************************************************/

Receiver::Receiver()
{
    /* Registers */
    REFLECT(r_slicer);

    /* Submodules */
    REFLECT(u_slicer);

    /* Nodes */
    REFLECT(i_clock);
    REFLECT(i_m_qam);
    REFLECT(i_p_rx);
    REFLECT(i_signal);
    REFLECT(o_signal);
}

/*******************************************************************************
* CONNECTIONS
********************************************************************************/

void Receiver::Connect()
{
    /* Registers */
    i_clock->RegisterOnPositiveEdge(this, r_slicer);

    /* Submodules */
    u_slicer.i_clock << i_clock;
    u_slicer.i_m_qam << i_m_qam;
    u_slicer.i_signal << r_slicer.o;
    
    /* Outputs */
    o_signal << u_slicer.o_signal;
}

/*******************************************************************************
* INITIALIZATION
********************************************************************************/

void Receiver::Init()
{
    m_qam = i_m_qam.GetData();
    p_rx = i_p_rx.GetData();
    p_qam = 2.0 / 3.0 * static_cast<double>(m_qam - 1UL);
}

/*******************************************************************************
* BEHAVIOR
********************************************************************************/

void Receiver::RunClockMaster()
{
    /* Power Normalization to Pqam */
    r_slicer.i = i_signal.GetData() * sqrt(p_qam / p_rx);
}