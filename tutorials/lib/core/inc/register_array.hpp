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
* Author: Mauro Venancio, Patricio Reus Merlo
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

#include "abstract_register.hpp"

/**
 * @brief Register class
 * This class models a register of type T.
 * It has an input and an output and some 
 * functionalities like Set, Reset and RunClock
 * 
 * @tparam T
 * @tparam N
 */
template <typename T, size_t N = 1>
class Register : public AbstractRegister
{
    
public:

    std::array<T, N> i { 0 };
    std::array<T, N> o { 0 };
    
    Register();
    Register(T data);
    Register(std::array<T, N>& data);

    void RunClock();
    void Reset();
    void Reset(size_t idx);
    void Set();
    void Set(T data);
    void Set(size_t idx);
    void Set(std::array<T, N>& data);
    void Set(T data, size_t idx);
};

template <class T, size_t N>
Register<T, N>::Register()
{
    REFLECT(i);
    REFLECT(o);
}

template <class T, size_t N>
Register<T, N>::Register(T data)
{
    for (size_t j { 0 }; j < N; j++)
    {
        this->i[j] = data;
        this->o[j] = data;
    }
    
    REFLECT(i);
    REFLECT(o);
}

template <class T, size_t N>
Register<T, N>::Register(std::array<T, N>& data) : i(data), o(data)
{
    REFLECT(i);
    REFLECT(o);
}

/**
 * @brief Sets the input and output data of the registers to the provided value.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 * @param data The value to set for both input and output.
 */
template <class T, size_t N>
void Register<T, N>::Set(T data)
{
    for (size_t j { 0 }; j < N; j++)
    {
        this->i[j] = data;
        this->o[j] = data;
    }
}

/**
 * @brief Sets the input and output data of the registers to 1.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 */
template <class T, size_t N>
void Register<T, N>::Set()
{
    for (size_t j { 0 }; j < N; j++)
    {
        this->i[j] = 1;
        this->o[j] = 1;
    }
}

/**
 * @brief Sets the input and output data of a specific register to 1.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 * @param idx Position of the register.
 */
template <class T, size_t N>
void Register<T, N>::Set(size_t idx)
{
    this->i[idx] = 1;
    this->o[idx] = 1;
}

/**
 * @brief Sets the input and output data of a specific register to the provided value.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 * @param data The value to set for both input and output.
 * @param idx Position of the register.
 */
template <class T, size_t N>
void Register<T, N>::Set(T data, size_t idx)
{
    this->i[idx] = data;
    this->o[idx] = data;
}

/**
 * @brief Sets the input and output data of the registers to the provided value.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 * @param data The value to set for both input and output.
 */
template <class T, size_t N>
void Register<T, N>::Set(std::array<T, N>& data)
{
    this->i = data;
    this->o = data;
}

/**
 * @brief Resets the input and output data of the registers to 0.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 */
template <class T, size_t N>
void Register<T, N>::Reset()
{
    for (size_t j { 0 }; j < N; j++)
    {
        this->i[j] = 0;
        this->o[j] = 0;
    } 
}

/**
 * @brief Resets the input and output data of a specific registers to 0.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 */
template <class T, size_t N>
void Register<T, N>::Reset(size_t idx)
{
    this->i[idx] = 0;
    this->o[idx] = 0; 
}

/**
 * @brief Advances the register on a clock cycle, 
 * updating the output with the input.
 *
 * @tparam T The data type.
 * @tparam N Array Length.
 */
template <class T, size_t N>
void Register<T, N>::RunClock()
{
    this->o = this->i;
}