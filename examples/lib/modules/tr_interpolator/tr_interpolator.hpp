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
* Author: Mauro Venancio
* Date: 06/20/2024
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

#include <complex>

#include "halcon.hpp"

template <size_t MU_DIVISION, size_t NTAPS, size_t FIFO_LENGHT>
class TRInterpolator : public Module
{
private:

    /* Registers */
    Register<std::complex<double>, FIFO_LENGHT> r_fifo;
    Register<std::complex<double>> r_out;

    /* Variables */
    std::complex<double> sum;
    double t_baud;
    size_t fifo_ptr;

    /* Settings YAML */
    double baudrate;
    double fs;
    double beta;
    bool bypass;

    /* Internal Functions */
    std::array<double, NTAPS> GenCoeffs(double offset);

    std::array<std::array<double, NTAPS>, MU_DIVISION> filter_bank;

public:

    TRInterpolator();

    /* Behavior */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;
    
    /* Ports */
    Input<Clock> i_clock;
    Input<std::complex<double>> i_signal;
    Input<double> i_offset;
    Input<int> i_base_pointer;
    Output<std::complex<double>> o_signal;
};

template <size_t MU_DIVISION, size_t NTAPS, size_t FIFO_LENGHT>
TRInterpolator<MU_DIVISION, NTAPS, FIFO_LENGHT>::TRInterpolator()
{
    /* Registers */
    REFLECT(r_fifo);
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_clock);
    REFLECT(i_signal);
    REFLECT(i_offset);
    REFLECT(i_base_pointer);
    REFLECT(o_signal);

    /* Settings YAML */
    REFLECT_YAML(baudrate);
    REFLECT_YAML(fs);
    REFLECT_YAML(beta);
    REFLECT_YAML(bypass);
}

template <size_t MU_DIVISION, size_t NTAPS, size_t FIFO_LENGHT>
void TRInterpolator<MU_DIVISION, NTAPS, FIFO_LENGHT>::Init()
{
    fifo_ptr = static_cast<size_t>(round(FIFO_LENGHT/2));

    t_baud = 1 / baudrate;

    for (size_t i = 0; i < MU_DIVISION; i++)
    {
        filter_bank[i] = GenCoeffs(-static_cast<double>(i) / MU_DIVISION);
    }
    
}

template <size_t MU_DIVISION, size_t NTAPS, size_t FIFO_LENGHT>
void TRInterpolator<MU_DIVISION, NTAPS, FIFO_LENGHT>::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_fifo);
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Outputs */
    o_signal << r_out.o;
}

template <size_t MU_DIVISION, size_t NTAPS, size_t FIFO_LENGHT>
void TRInterpolator<MU_DIVISION, NTAPS, FIFO_LENGHT>::RunClockMaster()
{
    for (size_t i { 0 }; i < (FIFO_LENGHT - 1); i++)
    {
        r_fifo.i[i] = r_fifo.o[i + 1];
    }
    r_fifo.i[FIFO_LENGHT - 1] = i_signal.GetData();
    
    size_t ptr = static_cast<size_t>(i_offset.GetData() * MU_DIVISION);

    sum = std::complex<double>(0, 0);
    for (size_t i = 0; i < NTAPS; i++)
    {
        sum += r_fifo.o[i_base_pointer.GetData() + fifo_ptr + i] * filter_bank[ptr][i];
    }

    r_out.i = sum;
}

template <size_t MU_DIVISION, size_t NTAPS, size_t FIFO_LENGHT>
std::array<double, NTAPS> TRInterpolator<MU_DIVISION, NTAPS, FIFO_LENGHT>::GenCoeffs(double offset)
{

    double pi = std::numbers::pi;
    double t { -0.5* NTAPS / fs + offset / fs};
    double coeffs_quadratic_sum { 0.0 };
    std::array<double, NTAPS> coeffs;

    auto epsilon = std::numeric_limits<double>::epsilon();

    for (size_t i = 0; i < NTAPS; i++)
    {
        if ((fabs(t - (t_baud/(2.0*beta))) > epsilon) && (fabs(t - (-t_baud/(2.0*beta))) > epsilon))
        {
            coeffs[i] = (std::sin(pi*t/t_baud)/(pi*t/t_baud)) * (std::cos(pi*beta*t/t_baud))/(1.0-(4.0*beta*beta*t*t/(t_baud*t_baud)));
        }
        else
        {
            coeffs[i] = (pi/4)*(std::sin(pi/(2.0*beta))/(pi/(2.0*beta)));
        }
        
        coeffs_quadratic_sum += (coeffs[i]*coeffs[i]);
        t += static_cast<double>(1/fs);
    }
   

    for (size_t i = 0; i < NTAPS; i++)
    {
        coeffs[i] /= sqrt(coeffs_quadratic_sum);
    }

    return coeffs;
}