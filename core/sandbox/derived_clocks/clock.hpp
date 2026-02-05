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
* Date: 07/01/2024
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

#pragma once

/*******************************************************************************
* STANDARD LIBRARIES
********************************************************************************/

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

/*******************************************************************************
* LOCAL LIBRARIES
********************************************************************************/

#include "time_accumulator.hpp"

/*******************************************************************************
* CLOCK CLASS
********************************************************************************/

class Clock
{
private:

    /* ID */
    std::string name;

    /* Edge control */
    enum EdgeType{NEGATIVE = 'n', POSITIVE = 'p'};
    char next_edge_type { POSITIVE };

    enum State{LOW = false, HIGH = true};
    bool state { LOW };

    /* Time control */
    TimeAccumulator time;
    long double full_period { 1 };
    long double half_period { 0.5 };
    long double last_edge_time { 0 };
    long double next_edge_time { 0 };
    unsigned long long n_ticks { 0 };
    unsigned long long n_edges { 0 };
    
    /* Derived clocks type */
    enum Type{INDEPENDENT = 'i', DEPENDENT = 'd', HYBRID = 'h'};
    char type { INDEPENDENT };

    /* Derived clocks (dependent side) */
    unsigned int division_factor_num { 1 };
    unsigned int division_factor_den { 1 };
    long double division_factor { 1 };

    /* Derived clocks (independent side) */
    friend class ClockLinker;
    std::vector<Clock*> derived_clocks;

    /* Settings */
    long double initial_phase { 0 };
    long double frequency_hz { 0 };

public:
    
    /* Constructor */
    Clock(std::string name, 
          long double frequency_hz,
          long double initial_phase, 
          unsigned int division_factor_num,
          unsigned int division_factor_den);

    /* Behavor */
    inline void Init();
    inline void RunClockMaster();
    inline void RunClockSlave();
    inline void Update();

    /* Getters */
    inline const char& GetType() const;
    inline const std::string& GetName() const;
    inline const long double& GetNextEdgeTime() const;
    inline const unsigned long long& GetTickCount() const;
    inline const unsigned long long& GetEdgeCount() const;

    /* Derived and Master clocks */
    void RegisterDerivedClock(Clock &derived_clock);
};

/*******************************************************************************
* CLOCK LINKER CLASS
********************************************************************************/

class ClockLinker
{
public:
    static void Synchronize(Clock* master, Clock* derived)
    {
        unsigned long long m_aux {master->n_edges  * derived->division_factor_den};
        unsigned long long d_aux {derived->n_edges * derived->division_factor_num};
        
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
        long double delta = derived->initial_phase / 360.0L * derived->full_period;
        derived->next_edge_time = derived->time.add(delta);

        /* Update derived */
        for (Clock* clock : derived->derived_clocks)
        {
            ClockLinker::Synchronize(derived, clock);
        }
    }

    static void MakeDerivative(Clock* master, Clock* derived)
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

        master->derived_clocks.push_back(derived);
    }

    static void Print(Clock* master, Clock* derived)
    {
        std::cout << " master : " << master->name           << " - derived: " << derived->name           << std::endl;
        std::cout << " next t : " << master->next_edge_time << " - derived: " << derived->next_edge_time << std::endl;
        std::cout << " n_edges: " << master->n_edges        << " - derived: " << derived->n_edges        << std::endl;

        bool cond = (master->n_edges * derived->division_factor_den == derived->n_edges * derived->division_factor_num);
        std::cout << " condition: " << cond << std::endl;
    }
};

/*******************************************************************************
* CLOCK METHODS
********************************************************************************/

Clock::Clock(std::string id, 
             long double freq_hz, long double phase_deg, 
             unsigned int factor_num, unsigned int factor_den)
{
    name = id;
    frequency_hz = freq_hz;
    initial_phase = phase_deg;

    full_period = 1 / frequency_hz;
    half_period = full_period / 2.0L;
    next_edge_time = time.add(initial_phase / 360.0L * full_period);
    
    /* Derived clock */
    division_factor_num = factor_num;
    division_factor_den = factor_den;
    division_factor = static_cast<long double>(factor_num)
                  / static_cast<long double>(factor_den);
}

inline void Clock::Init()
{
    /* Update derived */
    for (Clock* clock : derived_clocks)
    {
        ClockLinker::Synchronize(this, clock);
    }
}

inline void Clock::RunClockMaster()
{
    std::cout << "M_" << name << std::endl;
}

inline void Clock::RunClockSlave()
{
    std::cout << "S_" << name << std::endl;
}

void Clock::Update()
{
    /* Update master */
    n_edges++;
    state = !state;
    last_edge_time = next_edge_time;
    next_edge_time = time.add(half_period);
    next_edge_type = (state == LOW) ? POSITIVE : NEGATIVE;

    /* Update derived */
    for (Clock* clock : derived_clocks)
    {
        ClockLinker::Synchronize(this, clock);
    }

    std::cout << "n edges: " << n_edges << std::endl;

}

void Clock::RegisterDerivedClock(Clock &derived_clock)
{
    ClockLinker::MakeDerivative(this, &derived_clock);
}

inline const std::string& Clock::GetName() const
{
    return name;
}

inline const long double& Clock::GetNextEdgeTime() const
{
    return next_edge_time;
}

inline const unsigned long long& Clock::GetTickCount() const
{
    return n_ticks;
}

inline const unsigned long long& Clock::GetEdgeCount() const
{
    return n_edges;
}

inline const char& Clock::GetType() const
{
    return type;
}