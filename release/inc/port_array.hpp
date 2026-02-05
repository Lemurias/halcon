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

/*******************************************************************************
* STANDARD HEADERS
********************************************************************************/

#include <array>
#include <memory>

/*******************************************************************************
* LOCAL HEADERS
********************************************************************************/

#include "abstract_handler.hpp"
#include "abstract_port.hpp"
#include "handler.hpp"

/*******************************************************************************
* ARRAY PORT CLASS
********************************************************************************/

template<typename T, size_t N = 1>
class Port : public AbstractPort
{
private:

    std::array<Port<T>, N> ports;

public:

    /* Data access */
    std::array<T, N> GetData();
    T GetData(size_t idx);

    void SetData(std::array<T, N>& data);
    Port<T>& operator[](size_t i);
    
    /* Load data to variable */    
    template <typename U>
    void operator>>(std::array<U, N>& data);

    /* Point to another port or data */
    void operator<<(Port<T, N>& another);
    void operator<<(std::array<T, N>& data);
        
    /* Verification and Optimization */
    bool IsNull() override;
    bool Optimize() override;
    
    /* Handler */
    std::shared_ptr<AbstractHandler> GetHandlerPointer() override;
};

template<typename T, size_t N>
void Port<T, N>::SetData(std::array<T, N>& data)
{
    for (size_t i { 0 }; i < N; i++)
    {
        ports[i].SetData(data[i]);
    }
}

template<typename T, size_t N>
Port<T>& Port<T, N>::operator[](size_t i)
{
    return ports[i];
}

template<typename T, size_t N>
std::array<T, N> Port<T, N>::GetData()
{
    std::array<T, N> data;
    for (size_t i { 0 }; i < N; i++)
    {
        data[i] = ports[i].GetData();
    }
    return data;
}

template<typename T, size_t N>
T Port<T, N>::GetData(size_t idx)
{
    return ports[idx].GetData();
}

template<typename T, size_t N>
template <typename U>
void Port<T, N>::operator>>(std::array<U, N>& data)
{
    for (size_t i { 0 }; i < N; i++)
    {
        ports[i] >> data[i];
    }
}

template<typename T, size_t N>
void Port<T, N>::operator<<(Port<T, N>& another)
{
    for (size_t i { 0 }; i < N; i++)
    {
        ports[i] << another[i];
    }
}

template<typename T, size_t N>
void Port<T, N>::operator<<(std::array<T, N>& data)
{
    for (size_t i { 0 }; i < N; i++)
    {
        ports[i] << data[i];
    }
}

template<typename T, size_t N>
bool Port<T, N>::IsNull()
{
    bool is_null = true;

    for (size_t i { 0 }; i < N; i++)
    {
        is_null = is_null && ports[i].IsNull();
    }
    return is_null;
}

template<typename T, size_t N>
bool Port<T, N>::Optimize()
{
    for (size_t i { 0 }; i < N; i++)
    {
        ports[i].Optimize();
    }
    return this->IsNull();
}

/*******************************************************************************
* SPECIALIZATIONS
********************************************************************************/

/* Specialization of Handler<Port<T, N>>*/
#include "handler_port_array.hpp"

/* Specialization of Handler<Port<T, N>> with T = std::complex<T>*/
#include "handler_port_array_complex.hpp"

/* Specialization of Handler<Port<T, N>> with T = ac_fixed<W, I, S, Q, O>*/
#include "handler_port_array_ac_fixed.hpp"

/*******************************************************************************
* HANDLER GETTER
********************************************************************************/

template<typename T, size_t N>
std::shared_ptr<AbstractHandler> Port<T, N>::GetHandlerPointer()
{
    std::shared_ptr<AbstractHandler> ptr = std::make_shared<Handler<Port<T, N>>>();
    ptr->SetPointer(this);
    return ptr;
}