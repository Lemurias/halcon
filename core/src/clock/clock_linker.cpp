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
* Date: 07/02/2024
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

#include "clock_linker.hpp"
#include "clock.hpp"

/*******************************************************************************
* CLOCK LINKER CLASS
********************************************************************************/

void ClockLinker::Derivative(Clock* master, Clock* derived)
{
    /* From independent to derived */
    if (derived->type == Clock::INDEPENDENT)
    {
        derived->type = Clock::DEPENDENT;
    }

    /* From derived to hibrid */
    if (master->type == Clock::DEPENDENT)
    {
        master->type = Clock::HYBRID;
    }

    /* Connect derived clock frequency port */
    derived->i_frequency_hz << master->i_frequency_hz;

    /* Save derived clock */
    master->derived_clocks.push_back(derived);
}

void ClockLinker::Synchronize(Clock* master, Clock* derived)
{
    unsigned long long m_aux {master->n_edges  * derived->i_division_factor_den.GetData()};
    unsigned long long d_aux {derived->n_edges * derived->i_division_factor_num.GetData()};
    
    /* Skip */
    if (m_aux != d_aux)
    {
        return;
    }
    
    /* Copy master clock state */
    derived->time = master->time;
    derived->full_period = master->full_period * derived->division_factor;
    derived->half_period = master->half_period * derived->division_factor;   

    /* Update derived clock */
    long double delta = derived->phase_deg / 360.0L * derived->full_period;
    derived->next_edge_time = derived->time.Update(delta);

    /* Update derived */
    for (auto &clock : derived->derived_clocks)
    {
        ClockLinker::Synchronize(derived, clock);
    }
}