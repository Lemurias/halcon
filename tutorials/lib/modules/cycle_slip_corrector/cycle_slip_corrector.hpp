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
* Author: Mauro Nahuel Venancio
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

#include <complex>

#include "halcon.hpp"

template <size_t N>
class CycleSlipCorrector : public Module
{
private:

    /* Registers */
    Register<std::complex<double>, N> r_symb_ref;
    Register<std::complex<double>, N> r_symb_hat;
    Register<std::complex<double>> r_out;

    /* Variables */
    double m11;
    double m12;
    double phase_rotation;
    std::complex<double> symb_aux;

    /* Settings YAML */
    bool enable {false};

public:

    CycleSlipCorrector();

    /* User methods */
    void Init() override;
    void Connect() override;
    void RunClockMaster() override;

    /* Ports */
    Input<Clock> i_clock;

    Input<std::complex<double>> i_symb_ref;
    Input<std::complex<double>> i_symb_hat;
    Output<std::complex<double>> o_symb;
};

template <size_t N>
CycleSlipCorrector<N>::CycleSlipCorrector()
{
    /* Registers */
    REFLECT(r_out);
    REFLECT(r_symb_ref);
    REFLECT(r_symb_hat);

    /* Ports */
    REFLECT(i_symb_ref);
    REFLECT(i_symb_hat);
    REFLECT(o_symb);

    /* Variables */
    REFLECT(phase_rotation);

    /* Settings YAML */
    REFLECT_YAML(enable);
}

template <size_t N>
void CycleSlipCorrector<N>::Init()
{
    /* Pass */
}

template <size_t N>
void CycleSlipCorrector<N>::Connect()
{
    /* Register on clock positive edge */
    i_clock->RegisterOnPositiveEdge(this, r_out);
    i_clock->RegisterOnPositiveEdge(this, r_symb_ref);
    i_clock->RegisterOnPositiveEdge(this, r_symb_hat);

    /* Outputs */
    o_symb << r_out.o;
}

template <size_t N>
void CycleSlipCorrector<N>::RunClockMaster()
{
    if (enable)
    {
        /* Shift Registers */
        for (size_t i = (N - 1); i > 0; i--)
        {
            r_symb_ref.i[i] = r_symb_ref.o[i - 1];
            r_symb_hat.i[i] = r_symb_hat.o[i - 1];
        }
        r_symb_ref.i[0] = i_symb_ref.GetData();
        r_symb_hat.i[0] = i_symb_hat.GetData();

        /* Correlation */
        m11 = i_symb_ref.GetData().real() * i_symb_hat.GetData().real();
        m12 = i_symb_ref.GetData().real() * i_symb_hat.GetData().imag();
        for (size_t i = 0; i < N; i++)
        {
            m11 += r_symb_ref.o[i].real() * r_symb_hat.o[i].real();
            m12 += r_symb_ref.o[i].real() * r_symb_hat.o[i].imag();
        }
        
        /* Phase Correction */
        if (abs(m11) > abs(m12))
        {
            if (m11 > 0)
            {
                phase_rotation = 0;
            }
            else
            {
                phase_rotation = 2;
            }
        }
        else
        {
            if (m12 > 0)
            {
                phase_rotation = 1;
            }
            else
            {
                phase_rotation = 3;
            }
        }
        
        symb_aux = i_symb_hat.GetData() * exp(std::complex<double>(0, -1) * phase_rotation * M_PI_2);
        r_out.i = std::complex<double>(round(symb_aux.real()), round(symb_aux.imag()));
    }
    else
    {
        r_out.i =  i_symb_hat.GetData();
    }
}
