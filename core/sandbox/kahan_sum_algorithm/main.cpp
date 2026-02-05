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

#include <iostream>
#include <vector>
#include <limits>

#define INIT_TIME 1000.0L
#define FREQUENCY 20000e9L
#define N_ITERATIONS 200000000L
#define TIME_OK (INIT_TIME + (N_ITERATIONS / FREQUENCY))

long double kahanSum(long double delta);
long double kahan2Sum(long double delta);
long double kahanBabushkaNeumaierSum(long double delta);
long double KahanBabushkaKleinSum(long double delta);

int main()
{
    size_t n_iterations { N_ITERATIONS };
    long double frequency {FREQUENCY};
    long double period { 1.0L / FREQUENCY };
    long double time_ok { TIME_OK };
    
    long double sim_time_op_0 { INIT_TIME };
    long double sim_time_op_1 { 0 };
    long double sim_time_op_2 { 0 };
    long double sim_time_op_3 { 0 };
    long double sim_time_op_4 { 0 };

    long double sim_delta_op_0;
    long double sim_delta_op_1;
    long double sim_delta_op_2;
    long double sim_delta_op_3;
    long double sim_delta_op_4;

    std::cout.precision(std::numeric_limits<long double>::digits10 + 1);
    std::cout << std::scientific;

    for(size_t i {0}; i < N_ITERATIONS; i++)
    {
        sim_delta_op_0 = sim_time_op_0;
        sim_delta_op_1 = sim_time_op_1;
        sim_delta_op_2 = sim_time_op_2;
        sim_delta_op_3 = sim_time_op_3;
        sim_delta_op_4 = sim_time_op_4;

        sim_time_op_0 = sim_time_op_0 + period;
        sim_time_op_1 = kahanSum(period);
        sim_time_op_2 = kahan2Sum(period);
        sim_time_op_3 = kahanBabushkaNeumaierSum(period);
        sim_time_op_4 = KahanBabushkaKleinSum(period);

        sim_delta_op_0 = sim_time_op_0 - sim_delta_op_0;
        sim_delta_op_1 = sim_time_op_1 - sim_delta_op_1;
        sim_delta_op_2 = sim_time_op_2 - sim_delta_op_2;
        sim_delta_op_3 = sim_time_op_3 - sim_delta_op_3;
        sim_delta_op_4 = sim_time_op_4 - sim_delta_op_4;
    }

    std::cout << "Time 0: " << INIT_TIME << " - sim N : " << N_ITERATIONS  << " - freq : " << FREQUENCY / 1e9 << "[GHz]" << std::endl;
    std::cout << "Time  : " << time_ok       << " -     T : " << period << std::endl;
    std::cout << std::endl;
    std::cout << "Suma  : " << sim_time_op_0 << " - sim T : " << sim_delta_op_0 << std::endl;
    std::cout << "kahan1: " << sim_time_op_1 << " - sim T : " << sim_delta_op_1 << std::endl;
    std::cout << "kahan2: " << sim_time_op_2 << " - sim T : " << sim_delta_op_2 << std::endl;
    std::cout << "kahan3: " << sim_time_op_3 << " - sim T : " << sim_delta_op_3 << std::endl;
    std::cout << "Kahan4: " << sim_time_op_4 << " - sim T : " << sim_delta_op_4 << std::endl;
}

long double kahanSum(long double delta)
{
    static long double sum { INIT_TIME };
    static long double c { 0.0 };
    
    double y = delta - c;
    double t = sum + y;
    c = (t - sum) - y;
    sum = t;
    return sum;
}

long double kahan2Sum(long double delta)
{
    static long double sum { INIT_TIME };
    static long double c { 0.0 };
    
    long double y = delta + c;
    long double t = sum + y;
    long double z = t - sum;
    c = (sum - (t - z)) + (y - z);
    sum = t;
    return sum;
}

long double kahanBabushkaNeumaierSum(long double delta)
{
    static long double sum { INIT_TIME };
    static long double c { 0.0 };

    long double t = sum + delta;
    if (std::abs(sum) >= std::abs(delta))
    {
        c += (sum - t) + delta;
    }
    else
    {
        c += (delta - t) + sum;
    }

    sum = t;
    return sum + c;
}

long double KahanBabushkaKleinSum(long double delta)
{
    static long double sum { INIT_TIME };
    static long double cs { 0.0 };
    static long double ccs { 0.0 };
    static long double c { 0.0 };
    static long double cc { 0.0 };

    long double t = sum + delta;
    if (std::abs(sum) >= std::abs(delta))
    {
        c = (sum - t) + delta;
    }
    else
    {
        c = (delta - t) + sum;
    }
    
    sum = t;
    t = cs + c;
    if (std::abs(cs) >= std::abs(c))
    {
        cc = (cs - t) + c;
    }
    else
    {
        cc = (c - t) + cs;
    }
    
    cs = t;
    ccs = ccs + cc;
    return sum + cs + ccs;
}