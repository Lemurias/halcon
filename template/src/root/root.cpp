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



#include "root.hpp"
#include <fstream>

Root::Root()
{
    /* Clocks */
    REFLECT(clk);
    
    /* Modules */
    REFLECT(u_sin_low);
    REFLECT(u_sin_high);
    REFLECT(u_adder);
    REFLECT(u_filter);
    
    /* Settings YAML */
    REFLECT_YAML(n_iterations);
}

void Root::Connect()
{
    /* Scheduler */
    scheduler << clk;

    /* Modules */
    u_sin_low.i_clock << clk;
    u_sin_high.i_clock << clk;
    
    u_adder.i_low << u_sin_low.o_sin;
    u_adder.i_high << u_sin_high.o_sin;

    u_filter.i_clock << clk;
    u_filter.i_signal << u_adder.o_sum;
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
