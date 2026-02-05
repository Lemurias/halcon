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
* HEADERS
********************************************************************************/

#include <array>
#include <complex>
#include <cmath>
#include "halcon.hpp"

/*******************************************************************************
* MODULE
********************************************************************************/

template<typename T>
class SinGenerator : public Module
{
private:

    /* Registers */
    Register<T> r_out { 0 };
    
    /* Internal vars */
    double frequency_hz { 0 };
    double amplitude_v { 1 };
    double phase_deg { 0 };

public:

    SinGenerator();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Output<T> o_sin;
};

/*******************************************************************************
* CONSTRUCTOR (REFLECT MODULES, CLOCKS AND SETTINGS)
********************************************************************************/

template<typename T>
SinGenerator<T>::SinGenerator()
{
    /* Registers */
    REFLECT(r_out);
    
    /* Ports */
    REFLECT(o_sin);

    /* Settings YAML */
    REFLECT_YAML(frequency_hz);
    REFLECT_YAML(amplitude_v);
    REFLECT_YAML(phase_deg);
}

/*******************************************************************************
* CONNECTIONS
********************************************************************************/

template<typename T>
void SinGenerator<T>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Port */
    o_sin << r_out.o;
}

/*******************************************************************************
* INITIALIZATION
********************************************************************************/

template<typename T>
void SinGenerator<T>::Init()
{
    /* Pass */
}

/*******************************************************************************
* BEHAVIOR
********************************************************************************/

template<typename T>
void SinGenerator<T>::RunClockMaster()
{
    double pi = std::numbers::pi;
    std::complex<double> j(0, 1);

    double t = static_cast<double>(i_clock->GetTime());
    
    double omega = 2 * pi * frequency_hz;
    double phi = phase_deg * pi / 180.0;

    r_out.i = static_cast<T>(amplitude_v * std::sin(omega * t + phi));
}