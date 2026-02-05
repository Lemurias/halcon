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
* Author: Mauro Venancio, Patricio Reus Merlo
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

#include "root.hpp"

/*******************************************************************************
* CONSTRUCTOR (REFLECT MODULES, CLOCKS AND SETTINGS)
********************************************************************************/

Root::Root()
{
    /* Clocks */
    REFLECT(clk);

    /* Modules */
    REFLECT(u_ber_counter);
    REFLECT(u_channel);
    REFLECT(u_receiver);
    REFLECT(u_transmitter);

    /* Settings YAML */
    REFLECT_YAML(enable_log);
    REFLECT_YAML(p_tx);
    REFLECT_YAML(m_qam);
    REFLECT_YAML(n_iterations);
    REFLECT_YAML(br);
}

/*******************************************************************************
* CONNECTIONS
********************************************************************************/

void Root::Connect()
{
    /* Clock */
    clk.i_frequency_hz << br;
    clk.i_phase_deg.SetData(0);
    clk.i_division_factor_num.SetData(1);
    clk.i_division_factor_den.SetData(1);

    /* Command Handler Clock */
    clk_cmd_handler.i_frequency_hz << br;
    clk_cmd_handler.i_phase_deg.SetData(0);
    clk_cmd_handler.i_division_factor_num.SetData(1);
    clk_cmd_handler.i_division_factor_den.SetData(1);
    
    /* Modules */
    u_transmitter.i_clock << clk;
    u_transmitter.i_p_tx << p_tx;
    u_transmitter.i_m_qam << m_qam;

    u_channel.i_clock << clk;
    u_channel.i_signal << u_transmitter.o_tx;
    u_channel.i_n_ovr.SetData(1);
    u_channel.i_m_qam << m_qam;
    u_channel.i_p_tx << p_tx;
    
    u_receiver.i_clock << clk;
    u_receiver.i_m_qam << m_qam;
    u_receiver.i_p_rx << p_tx;
    u_receiver.i_signal << u_channel.o_signal;

    u_ber_counter.i_clock << clk;
    u_ber_counter.i_m_qam << m_qam;
    u_ber_counter.i_symb_hat << u_receiver.o_signal;
    u_ber_counter.i_symb_ref << u_transmitter.o_symb_ref;

}

/*******************************************************************************
* INITIALIZATION
********************************************************************************/

void Root::Init()
{
    /* pass */
}

/*******************************************************************************
* ITERATION
********************************************************************************/

void Root::Iteration()
{
    unsigned long c_iteration = GetIterationCounter();
    
    if (!(c_iteration % 1000) && enable_log)
    {
        std::cout << "-- Running: " << c_iteration << "/" << n_iterations << std::endl;
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
    std::cout << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "-- Simulation sample rate [kbps]: " << (GetIterationRate() / 1e3) << std::endl;
    std::cout << "-- Number of Bits: " << u_ber_counter.o_n_bits.GetData() << std::endl;
    std::cout << "-- Number of Errors: " << u_ber_counter.o_n_errors.GetData() << std::endl;
    std::cout << "-- BER: " << std::scientific << u_ber_counter.o_ber_value.GetData() << std::endl;
    std::cout << "-----------------------------" << std::endl;
}