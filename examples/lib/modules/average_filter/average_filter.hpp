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
* Date: 05/16/2024
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

template<typename T, size_t NTAPS>
class AverageFilter : public Module
{
private:
    /* Registers */
    Register<T, NTAPS - 1> r_shift_reg { 0 };
    Register<T> r_out { 0 };

    /* Internal vars */
    std::array<T, NTAPS> coeffs { 0 };
    T sum { 0 };
    bool normalize { true };

public:

    AverageFilter();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<T> i_signal;
    Input<bool> i_normalize;
    Output<T> o_signal;
};

template<typename T, size_t NTAPS>
AverageFilter<T, NTAPS>::AverageFilter()
{
    /* Registers */
    REFLECT(r_shift_reg);
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);


    /* Settings YAML */
    REFLECT_YAML(coeffs);
}

template<typename T, size_t NTAPS>
void AverageFilter<T, NTAPS>::Init()
{
    normalize = i_normalize.GetData();
    for (size_t i = 0; i < NTAPS; i++)
    {
        if (normalize)
        {
            coeffs[i] = static_cast<T>(1.0/(static_cast<double>(NTAPS)));
        }
        else
        {
            coeffs[i] = static_cast<T>(1.0);
        }
    }
}

template<typename T, size_t NTAPS>
void AverageFilter<T, NTAPS>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_shift_reg);
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Output Port */
    o_signal << r_out.o;
}

template<typename T, size_t NTAPS>
void AverageFilter<T, NTAPS>::RunClockMaster()
{
    /* Shift register */
    for(size_t i { 1 }; i < NTAPS - 1; ++i)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();

    /* Products */
    sum = i_signal.GetData() * coeffs[0];
    for(size_t i { 1 }; i < NTAPS; ++i)
    {
        sum += r_shift_reg.o[i - 1] * coeffs[i];
    }
    r_out.i = sum;
}
