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
* Author: Mauro Venancio
* Date: 17/07/2024
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

#include <array>
#include "halcon.hpp"

template<typename T, size_t NTAPS, size_t PARALLELISM>
class ParallelFilter : public Module
{
private:

    /* Registers */
    Register<T, (NTAPS - 1)> r_reg { 0 };

    /* Internal vars */
    std::array<T, NTAPS> coeffs { 0 };
    std::array<T, PARALLELISM> result { 0 };

public:

    ParallelFilter();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<T, PARALLELISM> i_signal;
    Output<T, PARALLELISM> o_signal;
};


template<typename T, size_t NTAPS, size_t PARALLELISM>
ParallelFilter<T, NTAPS, PARALLELISM>::ParallelFilter()
{
    /* Registers */
    REFLECT(r_reg);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);

    /* Variables */
    REFLECT(result);

    /* Settings YAML */
    REFLECT_YAML(coeffs);
}


template<typename T, size_t NTAPS, size_t PARALLELISM>
void ParallelFilter<T, NTAPS, PARALLELISM>::Init()
{
    /* Pass */
}



template<typename T, size_t NTAPS, size_t PARALLELISM>
void ParallelFilter<T, NTAPS, PARALLELISM>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_reg);

    /* Output */
    for (size_t i = 0; i < PARALLELISM; i++)
    {
        o_signal[(PARALLELISM - 1) - i] << result[i] << COMBINATIONAL_PORT;
    }
}


template<typename T, size_t NTAPS, size_t PARALLELISM>
void ParallelFilter<T, NTAPS, PARALLELISM>::RunClockMaster()
{
    /* ShiftRegister */
    for (size_t i = 0; i < (NTAPS - 1); i++)
    {
        if (i < PARALLELISM)
        {
            r_reg.i[i] = i_signal[i].GetData();
        }
        else
        {
            r_reg.i[i] = r_reg.o[i - PARALLELISM];
        }
    }
    

    /* Output */
    for (size_t i = 0; i < PARALLELISM; i++)
    {
        result[i] = 0;
        for (size_t j = 0; j < NTAPS; j++)
        {
            if (j <= i)
            {
                result[i] += i_signal[(PARALLELISM - 1) - (i - j)].GetData() * coeffs[j];
            }
            else
            {
                result[i] += r_reg.o[j - i - 1] * coeffs[j];
            }
        }
    }
}
