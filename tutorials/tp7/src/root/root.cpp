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
* Author: Patricio Reus Merlo
* Date: 05/21/2024
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

Root::Root()
{
    /* Clocks */
    REFLECT(clk);
    REFLECT(clk_slow);
    REFLECT(clk_fast);
    
    /* Modules */
    REFLECT(u_symbol_gen);
    REFLECT(u_upsampler);
    REFLECT(u_parallel_filter);
    REFLECT(u_ser_to_par);
    REFLECT(u_par_to_ser);

    /* Variables */
    REFLECT(iteration_rate);

    /* Settings YAML */
    REFLECT_YAML(enable_log);
    REFLECT_YAML(n_iterations);
    REFLECT_YAML(n_ovr);
}

void Root::Connect()
{
    /* Scheduler */
    scheduler << clk;
    scheduler << clk_slow;
    scheduler << clk_fast;

    /* Modules */
    u_symbol_gen.i_clock << clk_slow;
    u_symbol_gen.i_m_qam << m_qam;
    u_symbol_gen.i_p_tx << p_tx;

    u_upsampler.i_clock << clk_fast;
    u_upsampler.i_n_ovr << n_ovr;
    u_upsampler.i_signal << u_symbol_gen.o_symb_ref;

    u_ser_to_par.i_clock_slow << clk_slow;
    u_ser_to_par.i_clock_fast << clk_fast;
    u_ser_to_par.i_serial << u_upsampler.o_signal_i;

    u_parallel_filter.i_clock << clk_slow;
    u_parallel_filter.i_signal << u_ser_to_par.o_parallel;

    u_par_to_ser.i_clock_fast << clk_fast;
    u_par_to_ser.i_parallel << u_parallel_filter.o_signal;
};

void Root::Init()
{
    
}

void Root::Iteration()
{
    unsigned long c_iteration = GetIterationCounter();

    if (!(c_iteration % 1000UL) && enable_log)
    {
        std::cout << "-- Running: " << c_iteration << "/" << n_iterations << std::endl;
    }
}

bool Root::ContinueRunning()
{
    return GetIterationCounter() <= n_iterations;
}

void Root::Terminate()
{
    std::cout << "\n-----------------------------\n";
    std::cout << "Sample rate [kbps]: " << (GetIterationRate() / 1e3) << std::endl;
    std::cout << "-----------------------------\n";
}