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
* Date: 05/21/2024
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


template<typename InputType, typename OutputType, size_t Parallelism>
class ParallelToSerial : public Module
{
private:

    /* Registers */
    Register<size_t> r_counter { Parallelism - 1UL };
    Register<OutputType> r_out { 0 };

public:

    ParallelToSerial();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock_fast;
    Input<InputType, Parallelism> i_parallel;
    Output<OutputType> o_serial;
};

template<typename InputType, typename OutputType, size_t Parallelism>
ParallelToSerial<InputType, OutputType, Parallelism>::ParallelToSerial()
{
    /* Registers */
    REFLECT(r_out);
    REFLECT(r_counter);

    /* Ports */
    REFLECT(i_clock_fast);
    REFLECT(i_parallel);
    REFLECT(o_serial);
}

template<typename InputType, typename OutputType, size_t Parallelism>
void ParallelToSerial<InputType, OutputType, Parallelism>::Connect()
{
    /* Registers to Clock */
    i_clock_fast->RegisterOnPositiveEdge(this, r_out);
    i_clock_fast->RegisterOnPositiveEdge(this, r_counter);
    
    /* Output */
    o_serial << r_out.o;
}

template<typename InputType, typename OutputType, size_t Parallelism>
void ParallelToSerial<InputType, OutputType, Parallelism>::Init()
{
    
}

template<typename InputType, typename OutputType, size_t Parallelism>
void ParallelToSerial<InputType, OutputType, Parallelism>::RunClockMaster()
{
    /* MUX */
    size_t index = (Parallelism - 1UL) - r_counter.o;
    i_parallel[index] >> r_out.i;

    /* Counter */
    size_t next_count = r_counter.o + 1UL;
    r_counter.i = (next_count < Parallelism)? next_count : 0UL;
}