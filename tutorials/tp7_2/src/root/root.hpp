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

#include "ac_fixed.h"

#include "upsampler.hpp"
#include "symbol_generator.hpp"
#include "parallel_filter.hpp"
#include "serial_to_parallel.hpp"
#include "parallel_to_serial.hpp"
#include "average_filter.hpp"


#define PARALLELISM 4
#define NTAPS 4

class Root : public Simulator
{
public:
    
    Root();

private:

    /* FIXED POINT
        W: integer representing width of the type
        I: integer representing integer width
        S: bool parameter representing signedness
        Q: enumeration parameter for quantization
        O: enumeration parameter for overflow modes
    */

    using FXP_SER_TO_PAR_OUTPUT = ac_fixed<12, 7, true, AC_RND, AC_WRAP>;
    using FXP_PAR_TO_SER_INPUT = ac_fixed<12, 7, true, AC_RND, AC_WRAP>;
    using FXP_PAR_TO_SER_PARALLEL = ac_fixed<24, 14, true, AC_RND, AC_WRAP>;

    /* User methods */
    void Init() override;
    void Connect() override;
    void Iteration() override;
    bool ContinueRunning() override;
    void Terminate() override;

    /* Variables */
    float iteration_rate { 0 };
    
    /* Settings YAML */
    bool enable_log { false };
    size_t n_iterations { 100 };
    size_t n_ovr { 4 };
    size_t m_qam { 4 };
    double p_tx { 1.0 };
    bool normalize { true };

    /* Clocks */
    Clock clk;
    Clock clk_slow;
    Clock clk_fast;

    /* Modules */
    Upsampler u_upsampler;
    SymbolGenerator u_symbol_gen;
    SerialToParallel<double, double, PARALLELISM> u_ser_to_par;
    ParallelFilter<double, NTAPS, PARALLELISM> u_parallel_filter;
    ParallelToSerial<double, double, PARALLELISM> u_par_to_ser;
    AverageFilter<double, NTAPS> u_filter;
};