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

#include "clock.hpp"

/*******************************************************************************
* CLOCK CLASS
********************************************************************************/

/* Static attribute initialization */
std::vector<Clock*> Clock::instances;

Clock::Clock()
{
    /* Save instance pointer */
    instances.push_back(this);

    /* Variables */
    REFLECT(state);
    REFLECT(last_edge_time);
    REFLECT(next_edge_time);
    REFLECT(next_edge_type);
    REFLECT(n_ticks);
    REFLECT(n_edges);

    /* Ports */
    REFLECT(i_frequency_hz);
    REFLECT(i_phase_deg);
    REFLECT(i_division_factor_num);
    REFLECT(i_division_factor_den);
}

Clock::~Clock()
{
    auto it = std::find(instances.begin(), instances.end(), this);
    instances.erase(it);
}

void Clock::Init()
{
    /* Update period */
    division_factor_num = static_cast<long double>(i_division_factor_num.GetData());
    division_factor_den = static_cast<long double>(i_division_factor_den.GetData());
    division_factor = division_factor_num / division_factor_den;
    frequency = i_frequency_hz.GetData() / division_factor;
    full_period = 1 / frequency;
    half_period = 1 / frequency / 2;
    
    /* Update time */
    phase_deg = i_phase_deg.GetData();
    next_edge_time = time.Update(phase_deg / 360.0L * full_period);

    /* Update derived */
    for (auto &clock : derived_clocks)
    {
        ClockLinker::Synchronize(this, clock);
    }
}

void Clock::RegisterOnPositiveEdge(Module* module_ptr, AbstractRegister& reg_ref)
{
    posedge_map[module_ptr].push_back(&reg_ref);
    module_ptr->SetAsSequentialOrMixed();
}

void Clock::RegisterOnNegativeEdge(Module* module_ptr, AbstractRegister& reg_ref)
{
    negedge_map[module_ptr].push_back(&reg_ref);
    module_ptr->SetAsSequentialOrMixed();
}

void Clock::RegisterDerivedClock(Clock &master_clock)
{
    ClockLinker::Derivative(&master_clock, this);
}

void Clock::operator<<(Clock &master_clock)
{
    ClockLinker::Derivative(&master_clock, this);
}

void Clock::RunClockMaster()
{
    /* Run modules negative edge */
    #if RUN_POSEDGE_LOGIC_ONLY == 0
    for (auto &[module_ptr, reg_ptrs] : negedge_map)
    {
        if (module_ptr->IsMasterNotDone())
        {
            module_ptr->RunClockMaster();
            module_ptr->SetMasterAsDone();
        }
    }
    #endif

    /* Run modules positive edge */
    for(auto &[module_ptr, reg_ptrs] : posedge_map)
    {
        if (module_ptr->IsMasterNotDone())
        {
            module_ptr->RunClockMaster();
            module_ptr->SetMasterAsDone();
        }
    }
}

void Clock::RunClockSlave()
{
    /* Run negative edge */
    #if !RUN_POSEDGE_LOGIC_ONLY
    if (state == HIGH)
    {
        for (auto& [module_ptr, reg_ptrs] : negedge_map)
        {
            for (auto &reg_ptr : reg_ptrs)
            {
                reg_ptr->RunClock();
            }
            module_ptr->SetMasterAsNotDone();
        }
    }
    #endif

    /* Run positive edge */
    if (state == LOW)
    {
        for(auto &[module_ptr, reg_ptrs] : posedge_map)
        {
            for(auto &reg_ptr : reg_ptrs)
            {
                reg_ptr->RunClock();
            }
            module_ptr->SetMasterAsNotDone();
        }
    }
}

void Clock::Update()
{
    /* Update period */
    division_factor_num = static_cast<long double>(i_division_factor_num.GetData());
    division_factor_den = static_cast<long double>(i_division_factor_den.GetData());
    division_factor = division_factor_num / division_factor_den;
    frequency = i_frequency_hz.GetData() / division_factor;
    full_period = 1 / frequency;
    half_period = 1 / frequency / 2;

    /* Update time and edge */
    n_edges++;
    n_ticks += (state == LOW)? 1 : 0;
    last_edge_time = next_edge_time;

    #if !RUN_POSEDGE_LOGIC_ONLY
        state = !state;
        next_edge_time = time.Update(half_period);
        next_edge_type = (state == LOW) ? POSITIVE : NEGATIVE;
    #else
        next_edge_time = time.Update(full_period);
    #endif

    /* Update derived clocks */
    for (auto &clock : derived_clocks)
    {
        ClockLinker::Synchronize(this, clock);
    }
}