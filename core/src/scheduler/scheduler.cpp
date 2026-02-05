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

#include "scheduler.hpp"

/*******************************************************************************
* SCHEDULER CLASS
********************************************************************************/

void Scheduler::Init()
{
    /* Memory allocation */
    clocks.reserve(Clock::instances.size());

    /* Sort all Clock instances*/
    auto sorter = [](Clock* A, Clock* B)
    {
        return A->GetNextEdgeTime() < B->GetNextEdgeTime();
    };
    
    for(auto &clock : Clock::instances)
    {
        clocks.insert(std::lower_bound(clocks.begin(), clocks.end(), clock, sorter), clock);
    }

    /* Free memory */
    Clock::instances.clear();
    Clock::instances.shrink_to_fit();
}

void Scheduler::RunClocks()
{
    /* Clock master */
    for (Clock* clock : next_clocks)
    {
        clock->RunClockMaster();
    }

    /* Clock slave */
    for (Clock* clock : next_clocks)
    {
        clock->RunClockSlave();
        clock->Update();
    }

    /* Sort clocks */
    auto sorter = [](Clock* A, Clock* B)
    {
        return A->GetNextEdgeTime() < B->GetNextEdgeTime();
    };

    std::sort(clocks.begin(), clocks.end(), sorter);
}

void Scheduler::UpdateNextClocks()
{
    long double next_edge_time = clocks.front()->GetNextEdgeTime();
    unsigned int front_count { 0 };

    next_clocks.clear();

    for (Clock* clock : clocks)
    {
        /* Skip */
        if(clock->GetNextEdgeTime() > next_edge_time)
        {
            break;
        }

        /* Add to NextClocks */
        switch (clock->GetType())
        {
            /* Independent */
            case 'i':
                next_clocks.push_back(clock);
                break;
            
            /* Dependent */
            case 'd':
                next_clocks.push_front(clock);
                front_count ++;
                break;

            /* Hybrid */
            case 'h':
                next_clocks.insert(next_clocks.begin() + front_count, clock);
                break;
        }
    }
}