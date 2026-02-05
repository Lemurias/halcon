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

#include "register_array.hpp"

/**
 * @brief Register class (Specialization)
 * This class models a register of type T.
 * It has an input and an output and some 
 * functionalities like Set, Reset and RunClock
 * 
 * @tparam T
 */
template <typename T>
class Register<T, 1> : public AbstractRegister
{
    
public:

    T i{ 0 };
    T o{ 0 };

    Register();
    Register(T data);

    void RunClock() override;
    void Reset() override;
    void Set() override;
    void Set(T data);
};

template <class T>
Register<T, 1>::Register()
{
    REFLECT(i);
    REFLECT(o);
}

template <class T>
Register<T, 1>::Register(T data) : i(data), o(data)
{
    REFLECT(i);
    REFLECT(o);
}

/**
 * @brief Sets the input and output data of the register to the provided value.
 *
 * @tparam T The data type.
 * @param data The value to set for both input and output.
 */
template <class T>
void Register<T, 1>::Set(T data)
{
    this->i = data;
    this->o = data;
}

/**
 * @brief Sets the input and output data of the register to 1.
 *
 * @tparam T The data type.
 */
template <class T>
void Register<T, 1>::Set()
{
    this->i = 1;
    this->o = 1;
}

/**
 * @brief Resets the input and output data of the register to 0.
 *
 * @tparam T The data type.
 */
template <class T>
void Register<T, 1>::Reset()
{
    this->i = 0;
    this->o = 0;
}

/**
 * @brief Advances the register on a clock cycle, 
 * updating the output with the input.
 *
 * @tparam T The data type.
 */
template <class T>
void Register<T, 1>::RunClock()
{
    this->o = this->i;
}