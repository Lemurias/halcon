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
* Date: 05/17/2024
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
#include <complex>

inline const int QPSK  = 4 ;
inline const int QAM16 = 16;
inline const int QAM64 = 64;

/* QPSK */
inline std::array<std::complex<double>, 4> qpsk_symbols = 
{
    {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    }
};

inline std::array<std::array<bool, 2>, 4> qpsk_bits = 
{
    {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
    }
};

/* QAM16 */
inline std::array<std::complex<double>, 16> qam16_symbols = 
{
    {
        { 1, 3}, { 1, 1}, { 1, -1}, { 1, -3},
        { 3, 3}, { 3, 1}, { 3, -1}, { 3, -3},
        {-1, 3}, {-1, 1}, {-1, -1}, {-1, -3},
        {-3, 3}, {-3, 1}, {-3, -1}, {-3, -3}
    }
};

inline std::array<std::array<bool, 4>, 16> qam16_bits = 
{
    {
        {0, 0, 0, 1}, {0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 1},
        {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 1, 1},
        {1, 0, 0, 1}, {1, 0, 0, 0}, {1, 1, 0, 0}, {1, 1, 0, 1},
        {1, 0, 1, 1}, {1, 0, 1, 0}, {1, 1, 1, 0}, {1, 1, 1, 1}
    }
};

/* QAM64 */
inline std::array<std::complex<double>, 64> qam64_symbols = 
{
    {
        { 1, 3}, { 1, 1}, { 1, -1}, { 1, -3},
        { 3, 3}, { 3, 1}, { 3, -1}, { 3, -3},
        {-1, 3}, {-1, 1}, {-1, -1}, {-1, -3},
        {-3, 3}, {-3, 1}, {-3, -1}, {-3, -3},
        { 1, 7}, { 1, 5}, { 1, -5}, { 1, -7},
        { 3, 7}, { 3, 5}, { 3, -5}, { 3, -7},
        {-1, 7}, {-1, 5}, {-1, -5}, {-1, -7}, 
        {-3, 7}, {-3, 5}, {-3, -5}, {-3, -7}, 
        { 5, 3}, { 5, 1}, { 5, -1}, { 5, -3}, 
        { 7, 3}, { 7, 1}, { 7, -1}, { 7, -3}, 
        {-5, 3}, {-5, 1}, {-5, -1}, {-5, -3},
        {-7, 3}, {-7, 1}, {-7, -1}, {-7, -3},
        { 5, 7}, { 5, 5}, { 5, -5}, { 5, -7},
        { 7, 7}, { 7, 5}, { 7, -5}, { 7, -7},
        {-5, 7}, {-5, 5}, {-5, -5}, {-5, -7},
        {-7, 7}, {-7, 5}, {-7, -5}, {-7, -7}
    }
};

inline std::array<std::array<bool, 6>, 64> qam64_bits = 
{
    {
        {1, 1, 0, 1, 1, 1}, {1, 1, 0, 1, 1, 0}, {1, 1, 0, 0, 1, 0}, {1, 1, 0, 0, 1, 1},
        {1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 0}, {1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 1},
        {0, 1, 0, 1, 1, 1}, {0, 1, 0, 1, 1, 0}, {0, 1, 0, 0, 1, 0}, {0, 1, 0, 0, 1, 1},
        {0, 1, 1, 1, 1, 1}, {0, 1, 1, 1, 1, 0}, {0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 1}, 
        {1, 1, 0, 1, 0, 0}, {1, 1, 0, 1, 0, 1}, {1, 1, 0, 0, 0, 1}, {1, 1, 0, 0, 0, 0}, 
        {1, 1, 1, 1, 0, 0}, {1, 1, 1, 1, 0, 1}, {1, 1, 1, 0, 0, 1}, {1, 1, 1, 0, 0, 0}, 
        {0, 1, 0, 1, 0, 0}, {0, 1, 0, 1, 0, 1}, {0, 1, 0, 0, 0, 1}, {0, 1, 0, 0, 0, 0}, 
        {0, 1, 1, 1, 0, 0}, {0, 1, 1, 1, 0, 1}, {0, 1, 1, 0, 0, 1}, {0, 1, 1, 0, 0, 0}, 
        {1, 0, 1, 1, 1, 1}, {1, 0, 1, 1, 1, 0}, {1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 1}, 
        {1, 0, 0, 1, 1, 1}, {1, 0, 0, 1, 1, 0}, {1, 0, 0, 0, 1, 0}, {1, 0, 0, 0, 1, 1}, 
        {0, 0, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 0}, {0, 0, 1, 0, 1, 0}, {0, 0, 1, 0, 1, 1}, 
        {0, 0, 0, 1, 1, 1}, {0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 1, 1},
        {1, 0, 1, 1, 0, 0}, {1, 0, 1, 1, 0, 1}, {1, 0, 1, 0, 0, 1}, {1, 0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 0}, {1, 0, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0}, {0, 0, 1, 1, 0, 1}, {0, 0, 1, 0, 0, 1}, {0, 0, 1, 0, 0, 0}, 
        {0, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 0, 1}, {0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0}
    }
};