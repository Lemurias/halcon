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
* Author: Patricio Reus Merlo
* Date: 06/25/2024
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
* LOCAL HEADERS
********************************************************************************/

#include "root.hpp"

/*******************************************************************************
* ROOT CLASS
********************************************************************************/

Root::Root()
{
    /* Clocks */
    REFLECT(clk_ch);
    REFLECT(clk_br_tx);
    REFLECT(clk_br_rx);

    /* Modules */
    REFLECT(u_symbol_gen);
    REFLECT(u_upsampler);
    REFLECT(u_awgn_ch);
    REFLECT(u_downsampler);
    REFLECT(u_slicer);
    REFLECT(u_ber_counter);

    /* Variables */
    REFLECT(fs_ch);
    REFLECT(fs_br);
    REFLECT(p_qam);

    /* Settings YAML */
    REFLECT_YAML(enable_log);
    REFLECT_YAML(m_qam);
    REFLECT_YAML(n_symbols);
    REFLECT_YAML(n_ovr);
    REFLECT_YAML(br);
}

void Root::Connect()
{    
    /* Independent Clock */
    clk_ch.i_frequency_hz << fs_ch;
    clk_ch.i_phase_deg.SetData(0);
    clk_ch.i_division_factor_num.SetData(1);
    clk_ch.i_division_factor_den.SetData(1);

    /* Derived Clocks */
    clk_br_tx << clk_ch;
    clk_br_tx.i_phase_deg.SetData(0);
    clk_br_tx.i_division_factor_num << n_ovr;
    clk_br_tx.i_division_factor_den.SetData(1);

    clk_br_rx << clk_ch;
    clk_br_rx.i_phase_deg.SetData(0);
    clk_br_rx.i_division_factor_num << n_ovr;
    clk_br_rx.i_division_factor_den.SetData(1);

    /* Command Handler Clock */
    clk_cmd_handler << clk_ch;
    clk_cmd_handler.i_phase_deg.SetData(0);
    clk_cmd_handler.i_division_factor_num << n_ovr;
    clk_cmd_handler.i_division_factor_den.SetData(1);

    /* Modules */
    u_symbol_gen.i_clock << clk_br_rx;
    u_symbol_gen.i_m_qam << m_qam;
    u_symbol_gen.i_p_tx << p_qam;

    u_upsampler.i_clock << clk_ch;
    u_upsampler.i_n_ovr << n_ovr;
    u_upsampler.i_signal << u_symbol_gen.o_symb_ref;
    
    u_awgn_ch.i_clock << clk_ch;
    u_awgn_ch.i_n_ovr << n_ovr;
    u_awgn_ch.i_m_qam << m_qam;
    u_awgn_ch.i_p_tx << p_qam;
    u_awgn_ch.i_signal << u_upsampler.o_signal;

    u_downsampler.i_clock << clk_ch;
    u_downsampler.i_n_ovr << n_ovr;
    u_downsampler.i_signal << u_awgn_ch.o_signal;
    
    u_slicer.i_clock << clk_br_rx;
    u_slicer.i_m_qam << m_qam;
    u_slicer.i_signal << u_downsampler.o_signal;

    u_ber_counter.i_clock << clk_br_rx;
    u_ber_counter.i_m_qam << m_qam;
    u_ber_counter.i_symb_hat << u_slicer.o_signal;
    u_ber_counter.i_symb_ref << u_symbol_gen.o_symb_ref;
}

void Root::Init()
{
    fs_ch = br * static_cast<long double>(n_ovr);
    fs_br = br;

    p_qam = 2.0 / 3.0 * static_cast<double>(m_qam - 1UL);
}

void Root::Iteration()
{
    size_t c_n_symbols = u_symbol_gen.o_n_symbols.GetData();
    
    if (!(c_n_symbols % 1000) && enable_log)
    {
        std::cout << "-- Running: " << c_n_symbols << "/" << n_symbols << std::endl;
    }
}

bool Root::ContinueRunning()
{
    return u_symbol_gen.o_n_symbols.GetData() <= n_symbols;
}

void Root::Terminate()
{
    std::cout << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "-- Sample rate [kbps]: " << (GetIterationRate() / 1e3) << std::endl;
    std::cout << "-- Bits: " << u_ber_counter.o_n_bits.GetData() << std::endl;
    std::cout << "-- Errors: " << u_ber_counter.o_n_errors.GetData() << std::endl;
    std::cout << "-- BER: " << std::scientific << u_ber_counter.o_ber_value.GetData() << std::endl;
    std::cout << "-----------------------------" << std::endl;
}