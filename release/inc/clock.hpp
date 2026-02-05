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
* Date: 06/26/2024
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
* STANDARD HEADERS
********************************************************************************/

#include <unordered_map>
#include <vector>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "abstract_clock.hpp"
#include "abstract_register.hpp"
#include "clock_linker.hpp"
#include "module.hpp"
#include "port.hpp"
#include "time_accumulator.hpp"

/*******************************************************************************
* CLOCK CLASS
********************************************************************************/

class Clock : public AbstractClock
{
private:

    friend class Scheduler;
    static std::vector<Clock*> instances;

public:

    enum State{LOW = false, HIGH = true};
    enum EdgeType{NEGATIVE = 'n', POSITIVE = 'p'};
    enum Type{INDEPENDENT = 'i', DEPENDENT = 'd', HYBRID = 'h'};

protected:

    /* State and Edge */
    bool state { LOW };
    char next_edge_type { POSITIVE };

    /* Time and Frequency */
    TimeAccumulator time;
    long double phase_deg { 0 };
    long double frequency { 1 };
    long double division_factor_num { 1 };
    long double division_factor_den { 1 };
    long double division_factor { 1 };
    long double full_period { 1 };
    long double half_period { 0.5 };
    long double last_edge_time { 0 };
    long double next_edge_time { 0 };
    unsigned long long n_ticks { 0 };
    unsigned long long n_edges { 0 };
    
    /* Derived clocks */
    char type { INDEPENDENT };
    std::vector<Clock*> derived_clocks;
    friend class ClockLinker;
    
    /* Modules and registers */
    using RegistersVector = std::vector<AbstractRegister*>;
    using ModulePointer = Module*;
    using EdgeMap = std::unordered_map<ModulePointer, RegistersVector>;
    
    EdgeMap posedge_map;
    EdgeMap negedge_map;

public:

    Clock();
    ~Clock();

    /* Getters */
    inline const Clock* GetId();
    inline const char& GetType() const;
    inline const bool& GetState() const;
    inline const long double& GetTime() const;
    inline const char& GetNextEdgeType() const;
    inline const long double& GetNextEdgeTime() const;
    inline const long double& GetLastEdgeTime() const;
    inline const unsigned long long& GetTickCount() const;
    inline const unsigned long long& GetEdgeCount() const;
    
    /* Behavor */
    virtual void Init() override;
    void RunClockMaster() override;
    void RunClockSlave() override;
    void Update();

    /* Modules */
    void RegisterOnPositiveEdge(Module* module_ptr, AbstractRegister& reg_ref) override;
    void RegisterOnNegativeEdge(Module* module_ptr, AbstractRegister& reg_ref) override;

    /* Derived clocks */
    void RegisterDerivedClock(Clock &derived_clock);
    void operator<<(Clock &derived_clock);

    /* Inputs */
    Input<long double> i_frequency_hz;
    Input<long double> i_phase_deg;
    Input<unsigned long> i_division_factor_num;
    Input<unsigned long> i_division_factor_den;
};

inline const Clock* Clock::GetId()
{
    return this;
}

inline const char& Clock::GetType() const
{
    return type;
}

inline const bool& Clock::GetState() const
{
    return state;
}

inline const long double& Clock::GetTime() const
{
    return next_edge_time;
}

inline const char& Clock::GetNextEdgeType() const
{
    return next_edge_type;
}

inline const long double& Clock::GetNextEdgeTime() const
{
    return next_edge_time;
}

inline const long double& Clock::GetLastEdgeTime() const
{
    return last_edge_time;
}

inline const unsigned long long& Clock::GetTickCount() const
{
    return n_ticks;
}

inline const unsigned long long& Clock::GetEdgeCount() const
{
    return n_edges;
}

/*******************************************************************************
* HANDLER SPECIALIZATION
********************************************************************************/

/* Specialization of Handler<Clock> */
#include "handler_clock.hpp"

/* Specialization of Handler<Port<Clock>> */
#include "handler_port_clock.hpp"