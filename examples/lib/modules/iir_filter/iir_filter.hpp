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
* Date: 06/11/2024
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


template<typename T>
class IIRFilter : public Module
{
private:

    /* Registers */
    Register<T> r_out { 0 };

    /* Internal vars */
    double alpha { 0.01 };
    T y_t { 0 };

public:

    IIRFilter();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<T> i_signal;
    Output<T> o_signal;
};

template<typename T>
IIRFilter<T>::IIRFilter()
{
    /* Registers */
    REFLECT(r_out);

    /* Variables */
    REFLECT(y_t);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);


    /* Settings YAML */
    REFLECT_YAML(alpha);
}

template<typename T>
void IIRFilter<T>::Init()
{

}

template<typename T>
void IIRFilter<T>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Output Port */
    o_signal << y_t << COMBINATIONAL_PORT;
}

template<typename T>
void IIRFilter<T>::RunClockMaster()
{
    y_t = alpha * i_signal.GetData() + (1.0 - alpha) * r_out.o;

    r_out.i = y_t;
}
