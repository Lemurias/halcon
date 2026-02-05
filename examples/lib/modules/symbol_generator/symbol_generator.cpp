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

/*******************************************************************************
* HEADERS
********************************************************************************/

#include "symbol_generator.hpp"

/*******************************************************************************
* CONSTRUCTOR (REFLECT MODULES, CLOCKS AND SETTINGS)
********************************************************************************/

SymbolGenerator::SymbolGenerator()
{
    /* Registers */
    REFLECT(r_symb_tx);
    REFLECT(r_symb_ref);

    /* Ports */
    REFLECT(o_n_symbols);
    REFLECT(o_symb_tx);
    REFLECT(o_symb_ref);

    /* Variables */
    REFLECT(p_qam);
    REFLECT(k_qam);
    REFLECT(m_qam);
    REFLECT(p_tx);
    REFLECT(n_symbols);

    /* Settings YAML */
    REFLECT_YAML(seed);
}

/*******************************************************************************
* CONNECTIONS
********************************************************************************/

void SymbolGenerator::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_symb_tx);
    i_clock->RegisterOnPositiveEdge(this, r_symb_ref);

    /* Outputs */
    o_n_symbols << n_symbols;
    o_symb_tx << r_symb_tx.o;
    o_symb_ref << r_symb_ref.o;
}

/*******************************************************************************
* INITIALIZATION
********************************************************************************/

void SymbolGenerator::Init()
{
    m_qam = i_m_qam.GetData();
    p_tx = i_p_tx.GetData();

    /* Modulation Parameters */
    p_qam = 2.0 / 3.0 * (static_cast<double>(m_qam) - 1);
    k_qam = static_cast<int>(sqrt(m_qam));

    /* Random Generator */
    uniform_dist = std::uniform_int_distribution<int>(0, (k_qam - 1));
    rng = std::mt19937(seed);
}

/*******************************************************************************
* BEHAVIOR
********************************************************************************/

void SymbolGenerator::RunClockMaster()
{
    int symb_i_aux = 2 * uniform_dist(rng) - (k_qam - 1);
    int symb_q_aux = 2 * uniform_dist(rng) - (k_qam - 1);

    r_symb_ref.i = std::complex<double>(symb_i_aux, symb_q_aux);
    r_symb_tx.i = std::complex<double>(symb_i_aux, symb_q_aux) * sqrt(p_tx / p_qam);

    n_symbols++;
}