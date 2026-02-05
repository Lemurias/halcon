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
class SerialToParallel : public Module
{
private:

    /* Registers */
    Register<size_t> r_counter { Parallelism - 1 };
    Register<OutputType, Parallelism> r_buffer { 0 };

public:

    SerialToParallel();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock_fast;
    Input<Clock> i_clock_slow;
    Input<InputType> i_serial;
    Output<OutputType, Parallelism> o_parallel;
};

template<typename InputType, typename OutputType, size_t Parallelism>
SerialToParallel<InputType, OutputType, Parallelism>::SerialToParallel()
{
    /* Registers */
    REFLECT(r_buffer);
    REFLECT(r_counter);

    /* Ports */
    REFLECT(i_clock_fast);
    REFLECT(i_clock_slow);
    REFLECT(i_serial);
    REFLECT(o_parallel);
}

template<typename InputType, typename OutputType, size_t Parallelism>
void SerialToParallel<InputType, OutputType, Parallelism>::Init()
{
    /* pass */
}

template<typename InputType, typename OutputType, size_t Parallelism>
void SerialToParallel<InputType, OutputType, Parallelism>::Connect()
{
    /* Registers to Clock */
    i_clock_slow->RegisterOnPositiveEdge(this, r_buffer);
    i_clock_fast->RegisterOnPositiveEdge(this, r_counter);
 
    /* Output */
    o_parallel << r_buffer.o;
}

template<typename InputType, typename OutputType, size_t Parallelism>
void SerialToParallel<InputType, OutputType, Parallelism>::RunClockMaster()
{
    /* MUX */
    size_t index = (Parallelism - 1) - r_counter.o;
    i_serial >> r_buffer.i[index];

    /* Counter */
    size_t next_count = r_counter.o + 1UL;
    r_counter.i = (next_count < Parallelism)? next_count : 0UL;
}