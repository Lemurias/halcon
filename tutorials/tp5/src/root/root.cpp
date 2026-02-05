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


#include "root.hpp"
#include <fstream>

Root::Root()
{
    /* Clocks */
    REFLECT(clk);
    REFLECT(clk_oversampling);
    
    /* Modules */
    REFLECT(u_symbol_gen);
    REFLECT(u_rc_filter);
    REFLECT(u_upsampling);
    
    /* Settings YAML */
    REFLECT_YAML(n_iterations);
    REFLECT_YAML(p_tx);
    REFLECT_YAML(m_qam);
    REFLECT_YAML(oversampling);
    REFLECT_YAML(beta);
    REFLECT_YAML(baud_frequency_hz);
    REFLECT_YAML(normalize);
}

void Root::Connect()
{
    /* Scheduler */
    scheduler << clk;
    scheduler << clk_oversampling;

    /* Modules */
    u_symbol_gen.i_clock << clk;
    u_symbol_gen.i_p_tx << p_tx;
    u_symbol_gen.i_m_qam << m_qam;

    u_upsampling.i_clock << clk_oversampling;
    u_upsampling.i_n_ovr << oversampling;
    u_upsampling.i_signal << u_symbol_gen.o_symb_ref;

    u_rc_filter.i_clock << clk_oversampling;
    u_rc_filter.i_signal << u_upsampling.o_signal;
    u_rc_filter.i_beta << beta;
    u_rc_filter.i_baud_frequency_hz << baud_frequency_hz;
    u_rc_filter.i_oversampling << oversampling;
    u_rc_filter.i_normalize << normalize;
};

void Root::Init()
{
    PrintBigMap();
}

void Root::Iteration()
{
    if (!(GetIterationCounter() % 100)){
        std::cout << "-- Running : "
                  << GetIterationCounter() << "/" << n_iterations
                  << std::endl;
    }
}

bool Root::ContinueRunning()
{
    return GetIterationCounter() <= n_iterations;
}

void Root::Terminate()
{
    std::cout << "\n-----------------------------\n";
    std::cout << "Simulation sample rate [kbps]: " << (GetIterationRate() / 1e3) << std::endl;
    std::cout << "-----------------------------\n";
}
