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

/*******************************************************************************
* STANDARD LIBRARIES
********************************************************************************/

#include <iostream>

/*******************************************************************************
* LOCAL LIBRARIES
********************************************************************************/

#include "clock.hpp"
#include "scheduler.hpp"

/*******************************************************************************
* MAIN
********************************************************************************/

#define NAME(var) #var

int main()
{
    /* BaudRate */
    long double BRx1  { 2000e9L  };

    /* Oversampled frequencies */
    long double BRx64 { 64 * BRx1};
    long double BRx32 { 32 * BRx1};
    long double BRx16 { 16 * BRx1};
    long double BRx8  {  8 * BRx1};
    long double BRx4  {  4 * BRx1};
    long double BRx2  {  2 * BRx1};
    
    long double BRx1_125 { 1.125 * BRx1};

    /* Scheduler */
    Scheduler sch;

    /* Clocks */
    // Clock clk_BRx64("clk_BRx64", BRx64, 0,  1, 1);
    // Clock clk_BRx32("clk_BRx32", BRx32, 0,  2, 1);
    // Clock clk_BRx16("clk_BRx16", BRx16, 0,  4, 1);
    // Clock clk_BRx8 ("clk_BRx8",  BRx8,  0,  8, 1);
    // Clock clk_BRx4 ("clk_BRx4",  BRx4,  0, 16, 1);
    Clock clk_BRx2 ("clk_BRx2",  BRx2,  0, 1, 1);
    // Clock clk_BRx1 ("clk_BRx1",  BRx1,  0, 64, 1);

    Clock clk_BRx1_125 ("clk_BRx1_125",  BRx1_125,  0, 16, 9);
    
    /* Scheduler list */
    // sch << clk_BRx64;
    // sch << clk_BRx32;
    // sch << clk_BRx16;
    // sch << clk_BRx8;
    // sch << clk_BRx4;
    sch << clk_BRx2;
    // sch << clk_BRx1;
    
    sch << clk_BRx1_125;

    /* Link clocks */
    // clk_BRx64.RegisterDerivedClock(clk_BRx32);
    // clk_BRx64.RegisterDerivedClock(clk_BRx16);
    // clk_BRx64.RegisterDerivedClock(clk_BRx8);
    // clk_BRx64.RegisterDerivedClock(clk_BRx4);
    // clk_BRx64.RegisterDerivedClock(clk_BRx2);
    // clk_BRx64.RegisterDerivedClock(clk_BRx1);

    clk_BRx2.RegisterDerivedClock(clk_BRx1_125);

    /*  */
    // clk_BRx64.Init();
    // clk_BRx32.Init();
    // clk_BRx16.Init();
    // clk_BRx8.Init();
    // clk_BRx4.Init();
    clk_BRx2.Init();
    // clk_BRx1.Init();

    clk_BRx1_125.Init();

    /* Run scheduler */
    for(size_t i {0}; i < 100; i++)
    {
        std::cout << std::endl << "Tick N: " << i << std::endl;
        std::cout << "==================================" << std::endl;
        sch.UpdateNextClocks();
        sch.RunClocks();
    }
}