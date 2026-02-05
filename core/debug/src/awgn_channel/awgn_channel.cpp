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

#include "awgn_channel.hpp"

AWGNChannel::AWGNChannel()
{
    /* Registers */
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_clock);
    REFLECT(i_n_ovr);
    REFLECT(i_m_qam);
    REFLECT(i_p_tx);
    REFLECT(i_signal);
    REFLECT(o_signal);

    /* Variables */
    REFLECT(p_tx);
    REFLECT(snr_lin);
    REFLECT(p_noise);
    REFLECT(n_ovr);
    REFLECT(m_qam);

    /* Settings YAML */
    REFLECT_YAML(ebno_db);
    REFLECT_YAML(seed);
}

void AWGNChannel::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Outputs */
    o_signal << r_out.o;
}

void AWGNChannel::Init()
{
    n_ovr = i_n_ovr.GetData();
    m_qam = i_m_qam.GetData();
    p_tx = i_p_tx.GetData();

    /* Noise Power */
    snr_lin = std::pow(10, ebno_db / 10) * std::log2(m_qam) / static_cast<double>(n_ovr);
    p_noise = p_tx / snr_lin;

    /* Random Generator */
    normal_dist = std::normal_distribution<double>(0, 1);
    rng = std::mt19937(seed);
}

void AWGNChannel::RunClockMaster()
{
    double noise_i = sqrt(p_noise / 2) * normal_dist(rng);
    double noise_q = sqrt(p_noise / 2) * normal_dist(rng);

    r_out.i = std::complex<double>(noise_i, noise_q) + i_signal.GetData();
}