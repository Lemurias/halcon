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
* Date: 05/22/2024
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

#include "halcon.hpp"

template<typename T>
class Counter : public Module
{
private:

    /* Registers */
    Register<T> r_counter { 0 };
    
    /* Variables */
    T step { 0 };
    T min_count { 0 };
    T max_count { 0 };

public:

    Counter();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<T> i_step;
    Input<T> i_init;
    Input<T> i_min_count;
    Input<T> i_max_count;
    Output<T> o_count;
};

template<typename T>
Counter<T>::Counter()
{
    /* Registers */
    REFLECT(r_counter);
    
    /* Ports */
    REFLECT(i_clock);
    REFLECT(i_step);
    REFLECT(i_init);
    REFLECT(i_min_count);
    REFLECT(i_max_count);
    REFLECT(o_count);
}

template<typename T>
void Counter<T>::Connect()
{
    /* Registers to Clock */
    i_clock->RegisterOnPositiveEdge(this, r_counter);
    
    /* Output */
    o_count << r_counter.o;
}

template<typename T>
void Counter<T>::Init()
{
    i_step >> step;
    i_init >> r_counter.i;
    i_min_count >> min_count;
    i_max_count >> max_count;
}

template<typename T>
void Counter<T>::RunClockMaster()
{
    T next_count = r_counter.o + step;
    
    if (next_count < max_count && next_count > min_count)
    {
        r_counter.i = next_count;
    }
    else
    {
        r_counter.i = min_count;
    }
}