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
* Date: 05/23/2024
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

#include <cmath>


template<typename T, size_t NTAPS>
class RCFilter : public Module
{
private:

    /* Registers */
    Register<T, NTAPS - 1> r_shift_reg { 0 };
    Register<T> r_out { 0 };

    /* Internal vars */
    std::array<double, NTAPS> coeffs;
    T sum { 0 };
    double beta;
    int n_bauds;
    double t_baud;
    int oversampling;
    bool normalize;

public:

    RCFilter();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;
    Input<T> i_signal;
    Input<double> i_beta;
    Input<double> i_baud_frequency_hz;
    Input<size_t> i_oversampling;
    Input<bool> i_normalize;
    Output<T> o_signal;
    Output<T, NTAPS> o_shift_reg;
};

template<typename T, size_t NTAPS>
RCFilter<T, NTAPS>::RCFilter()
{
    /* Registers */
    REFLECT(r_shift_reg);
    REFLECT(r_out);

    /* Ports */
    REFLECT(i_signal);
    REFLECT(o_signal);
    REFLECT(o_shift_reg);

    /* Settings YAML */
    REFLECT_YAML(coeffs);
}

template<typename T, size_t NTAPS>
void RCFilter<T, NTAPS>::Init()
{
    double pi = std::numbers::pi;
    beta = i_beta.GetData();
    oversampling = static_cast<int>(i_oversampling.GetData());
    n_bauds = static_cast<int>(NTAPS/oversampling);
    t_baud = 1.0/(i_baud_frequency_hz.GetData());
    normalize = i_normalize.GetData();
    double t { -0.5*n_bauds*t_baud };
    double coeffs_quadratic_sum { 0.0 };

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
        t += static_cast<double>(t_baud/oversampling);
    }

    if (normalize == true)
    {
        for (size_t i = 0; i < NTAPS; i++)
        {
            coeffs[i] /= sqrt(coeffs_quadratic_sum);
        }
    }
}

template<typename T, size_t NTAPS>
void RCFilter<T, NTAPS>::Connect()
{
    /* Register the module into the clock as positive edge sensitive */
    i_clock->RegisterOnPositiveEdge(this, r_shift_reg);
    i_clock->RegisterOnPositiveEdge(this, r_out);

    /* Output Port */
    o_signal << r_out.o;

    for(size_t i { 1 }; i < NTAPS; ++i)
    {
        o_shift_reg[i] << r_shift_reg.o[i - 1];
    }
    o_shift_reg[0] << i_signal;
}

template<typename T, size_t NTAPS>
void RCFilter<T, NTAPS>::RunClockMaster()
{
    /* Shift register */
    for(size_t i { 1 }; i < NTAPS - 1; ++i)
    {
        r_shift_reg.i[i] = r_shift_reg.o[i - 1];
    }
    r_shift_reg.i[0] = i_signal.GetData();

    /* Products */
    sum = i_signal.GetData() * static_cast<T>(coeffs[0]);
    for(size_t i { 1 }; i < NTAPS; ++i)
    {
        sum += r_shift_reg.o[i - 1] * static_cast<T>(coeffs[i]);
    }
    r_out.i = sum;
}
