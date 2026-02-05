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
* Date: 05/20/2024
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

#include "ber_counter.hpp"

BERCounter::BERCounter()
{
    /* Registers */
    REFLECT(r_delay_comp);
    REFLECT(r_n_bits);
    REFLECT(r_n_errors);
    REFLECT(r_ber_value);

    /* Ports */
    REFLECT(i_clock);
    REFLECT(i_m_qam);
    REFLECT(i_symb_hat);
    REFLECT(i_symb_ref);
    REFLECT(o_n_errors);
    REFLECT(o_n_bits);
    REFLECT(o_ber_value);

    /* Variables */
    REFLECT(phase);
    REFLECT(valid_phase);
    
    /* Settings YAML */
    REFLECT_YAML(enable);
    REFLECT_YAML(corr_signals_size);
}

void BERCounter::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_delay_comp);
    i_clock->RegisterOnPositiveEdge(this, r_n_errors);
    i_clock->RegisterOnPositiveEdge(this, r_n_bits);
    i_clock->RegisterOnPositiveEdge(this, r_ber_value);

    /* Outputs */
    o_n_errors << r_n_errors.o;
    o_n_bits << r_n_bits.o;
    o_ber_value << r_ber_value.o;
}

void BERCounter::Init()
{
    m_qam = i_m_qam.GetData();

    bits_hat.resize(std::log2(m_qam), 0.0);
    bits_ref.resize(std::log2(m_qam), 0.0);
}

void BERCounter::RunClockMaster()
{
    /* Optimal Phase Estimation */
    if (enable && !valid_phase)
    {
        if(correlation_counter < corr_signals_size)
        {
            symbols_hat.push_back(i_symb_hat.GetData().real());
            symbols_ref.push_back(i_symb_ref.GetData().real());
            correlation_counter++;
        }
        else
        {
            phase = ComputeOptimalPhase();
            valid_phase = true;
            correlation_counter = 0;
            symbols_hat.clear();
            symbols_hat.shrink_to_fit();
            symbols_ref.clear();
            symbols_ref.shrink_to_fit();
        }
    }

    /* Bit Error Rate Estimation*/
    if (enable && valid_phase)
    {   
        /* Updated Buffer */
        for (size_t i = 99; i > 0; i--)
        {
            r_delay_comp.i[i] = r_delay_comp.o[i - 1];
        }
        r_delay_comp.i[0] = i_symb_ref.GetData();
        
        if (r_delay_comp.o[phase - 1] != std::complex<double>(0, 0))
        {
            /* Demapper */
            bits_ref = Demapper(r_delay_comp.o[phase - 1]);
            bits_hat = Demapper(i_symb_hat.GetData());

            /* Comparision */
            size_t n_errors = 0;
            for (size_t i = 0; i < log2(m_qam); i++)
            {
                n_errors += (bits_ref[i] ^ bits_hat[i]);
            }
            
            /* Counter */
            r_n_bits.i = r_n_bits.o + static_cast<size_t>(log2(m_qam));
            r_n_errors.i = r_n_errors.o + n_errors;
            r_ber_value.i = static_cast<double>(r_n_errors.o) / static_cast<double>(r_n_bits.o);
        }
        
    }
    else
    {
        r_n_errors.i = 0;
        r_n_bits.i = 0;
        r_ber_value.i = 0;
    }

    
}

std::vector<bool> BERCounter::Demapper(std::complex<double> symbol)
{
    std::vector<bool> demod_symb(log2(m_qam));
    
    if (m_qam == QPSK)
    {
        auto it  = std::find(qpsk_symbols.begin(), qpsk_symbols.end(), symbol);
        std::size_t idx = std::distance(qpsk_symbols.begin(), it);
        std::copy(qpsk_bits[idx].begin(), qpsk_bits[idx].end(), demod_symb.begin());
    }
    
    if (m_qam == QAM16)
    {
        auto it  = std::find(qam16_symbols.begin(), qam16_symbols.end(), symbol);
        std::size_t idx = std::distance(qam16_symbols.begin(), it);
        std::copy(qam16_bits[idx].begin(), qam16_bits[idx].end(), demod_symb.begin());
    }

    if (m_qam == QAM64)
    {
        auto it = std::find(qam64_symbols.begin(), qam64_symbols.end(), symbol);
        std::size_t idx = std::distance(qam64_symbols.begin(), it);
        std::copy(qam64_bits[idx].begin(), qam64_bits[idx].end(), demod_symb.begin());
    }
    
    return demod_symb;
}

size_t BERCounter::ComputeOptimalPhase()
{
    /* Reverse one input */
    std::reverse(std::begin(symbols_ref), std::end(symbols_ref));

    /* Convolution */
    size_t corr_length = 2 * corr_signals_size - 1;
    std::vector<double> convolution(corr_length, 0.0);
    
    for (size_t i { 0 }; i < corr_length; ++i)
    {
        for (size_t j { 0 }; j < corr_signals_size; ++j)
        {
            if (i >= j && (i - j) < corr_signals_size) 
            {
                convolution[i] += symbols_hat[j] * symbols_ref[i - j];
            }
        }
    }

    /* Compute Squared Module */
    auto abs = [](const std::complex<double>& c)
    {
        return std::abs(c) * std::abs(c);
    };

    transform(convolution.begin(), convolution.end(), convolution.begin(), abs);

    /* Discrete time */
    std::vector<int> discrete_time;
    for (int i {- static_cast<int>(corr_signals_size) + 1}; i < static_cast<int>(corr_signals_size); i++)
    {
        discrete_time.push_back(i);
    }

    /* Peak detection */
    auto max_iter = std::max_element(convolution.begin(), convolution.end());
    size_t max_index = std::distance(convolution.begin(), max_iter);

    return discrete_time[max_index];
}