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
* Date: 06/16/2024
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

#include "root.hpp"
#include <fstream>

/*******************************************************************************
* CONSTRUCTOR (REFLECT MODULES, CLOCKS AND SETTINGS)
********************************************************************************/

Root::Root()
{
    /* Clocks */
    REFLECT(clk);
    
    /* Modules */
    REFLECT(u_sin_gen);
    REFLECT(u_shift_reg);
    
    /* Settings YAML */
    REFLECT_YAML(enable_log);
    REFLECT_YAML(n_iterations);
    REFLECT_YAML(fs_hz);
}

/*******************************************************************************
* CONNECTIONS
********************************************************************************/

void Root::Connect()
{
    /* Clock */
    clk.i_frequency_hz << fs_hz;
    clk.i_phase_deg.SetData(0);
    clk.i_division_factor_num.SetData(1);
    clk.i_division_factor_den.SetData(1);

    /* Command Handler Clock */
    clk_cmd_handler.i_frequency_hz << fs_hz;
    clk_cmd_handler.i_phase_deg.SetData(0);
    clk_cmd_handler.i_division_factor_num.SetData(1);
    clk_cmd_handler.i_division_factor_den.SetData(1);

    /* Modules */
    u_sin_gen.i_clock << clk;
    
    u_shift_reg.i_clock << clk;
    u_shift_reg.i_signal << u_sin_gen.o_sin;
};

/*******************************************************************************
* INITIALIZATION
********************************************************************************/

void Root::Init()
{
    std::cout << "-- Simulation started\n";
}

/*******************************************************************************
* ITERATION
********************************************************************************/

void Root::Iteration()
{
    unsigned long c_iteration = GetIterationCounter();

    if (!(c_iteration % 1000UL) && enable_log)
    {
        std::cout << "-- Running: "
                  << c_iteration << "/" << n_iterations
                  << std::endl;
    }
}

bool Root::ContinueRunning()
{
    return GetIterationCounter() <= n_iterations;
}

/*******************************************************************************
* TERMINATION
********************************************************************************/

void Root::Terminate()
{
    std::cout << "-- Simulation finished\n\n";

    std::cout << "Sample rate [kbps]: " << (GetIterationRate() / 1e3) << std::endl;
}