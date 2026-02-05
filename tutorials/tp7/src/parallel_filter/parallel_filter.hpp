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
* Author: Manuel Exequiel Arias
* Date: 06/03/2024
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

// #define NTAPS 5

template<typename T, size_t NTAPS>
class ParallelFilter : public Module
{
private:

    /* Registers */
    Register<T, (4 - 1)> r_reg { 0 };

    /* Internal vars */
    std::array<T, 4> coeffs {1, 1, 1, 1};
    std::array<T, 4> x_t { 0 };
    std::array<T, 4> y_t { 0 };

public:

    ParallelFilter();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<T, 4> i_signal;
    Output<T, 4> o_signal;
};


template<typename T, size_t NTAPS>
ParallelFilter<T, NTAPS>::ParallelFilter()
{
    /* Registers */
    REFLECT(r_reg);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);

    /* Settings YAML */
    REFLECT_YAML(coeffs);
    REFLECT(x_t);
    REFLECT(y_t);
}


template<typename T, size_t NTAPS>
void ParallelFilter<T, NTAPS>::Init()
{

}


template<typename T, size_t NTAPS>
void ParallelFilter<T, NTAPS>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_reg);
    // i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Output Port */
    for (size_t i = 0; i < 4; i++)
    {
        o_signal[i] << y_t[i] << COMBINATIONAL_PORT;
    }
}


template<typename T, size_t NTAPS>
void ParallelFilter<T, NTAPS>::RunClockMaster()
{
    /* Input assign */
    for (size_t i = 0; i < 4; i++)
    {
        i_signal[i] >> x_t[i];
    }

    /* Register */
    for (size_t i = 0; i < (4 - 1); i++)
    {
        r_reg.i[i] = x_t[i];
    }

    /* Output calc */
    y_t[0] = x_t[0] * coeffs[0] +     x_t[1] * coeffs[1] +     x_t[2] * coeffs[2] +     x_t[3] * coeffs[3];
    
    y_t[1] = x_t[1] * coeffs[0] +     x_t[2] * coeffs[1] +     x_t[3] * coeffs[2] + r_reg.o[0] * coeffs[3];

    y_t[2] = x_t[2] * coeffs[0] +     x_t[3] * coeffs[1] + r_reg.o[0] * coeffs[2] + r_reg.o[1] * coeffs[3];
    
    y_t[3] = x_t[3] * coeffs[0] + r_reg.o[0] * coeffs[1] + r_reg.o[1] * coeffs[2] + r_reg.o[2] * coeffs[3];

}
