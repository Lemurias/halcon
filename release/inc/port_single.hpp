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
* LOCAL HEADERS
********************************************************************************/

#include "port_array.hpp"
#include "module.hpp"

/*******************************************************************************
* ALIAS
********************************************************************************/

template<class T, size_t N = 1>
using Input = Port<T, N>;

template<class T, size_t N = 1>
using Output = Port<T, N>;

/*******************************************************************************
* SINGLE PORT CLASS
********************************************************************************/

template <typename T>
class Port <T, 1> : public AbstractPort
{
private:

    /* Port definition */
    T* p_data { nullptr };
    Port<T>* p_port { nullptr };
    Module* p_module { nullptr };

    /* Control variables */
    bool p_data_is_dynamic { false };

public:

    Port() = default;
    ~Port();

    /* Data access */
    T GetData();
    void SetData(T data);
    
    /* Load data to variable */
    template <typename U>
    void LoadDataTo(U& data);
    
    template <typename U>
    void operator>>(U& data);

    /* Run methods through port */
    T* GetDataPointer();
    T* operator->();

    /* Point to another port */
    void PointTo(Port<T>& another);
    void operator<<(Port<T>& another);
    
    /* Point to data */
    Port<T>& PointTo(T& data);
    Port<T>& operator<<(T& data);

    /* Combinationals */
    void SetModulePtr(Module* pointer);
    void operator<<(Module* pointer);
    Module* GetModulePointer();

    /* Verification and Optimization */
    bool IsNull() override;
    bool Optimize() override;

    /* Handler */
    std::shared_ptr<AbstractHandler> GetHandlerPointer() override;
};

template<typename T>
Port<T, 1>::~Port()
{
    if(p_data_is_dynamic)
    {
        delete p_data;
    }
}

template<typename T>
T Port<T, 1>::GetData()
{
    /* Port pointing to another Port */
    if (p_port)
    {
        return p_port->GetData();
    }

    /* Combinational Port */
    if (p_module && (p_module->IsFullyCombinational() || p_module->IsMasterNotDone()))
    {   
        p_module->RunClockMaster();
        p_module->SetMasterAsDone();
    }

    /* Port without connection return 0 by default */
    if (p_data == nullptr)
    {
        return static_cast<T>(0);
    }

    /* Port pointing to data */
    return *(p_data);
}

template<typename T>
void Port<T, 1>::SetData(T data)
{
    if (p_data != nullptr)
    {
        *this->p_data = data;
        return;
    }
    
    p_data = new T(data);
    p_data_is_dynamic = true;
}

template<typename T>
template <typename U>
void Port<T, 1>::LoadDataTo(U& data)
{
    data = static_cast<U>(this->GetData());
}

template<typename T>
template <typename U>
void Port<T, 1>::operator>>(U& data)
{
    data = static_cast<U>(this->GetData());
}

template<typename T>
T* Port<T, 1>::GetDataPointer()
{
    return (p_port == nullptr)? p_data : p_port->GetDataPointer();
}

template<typename T>
T* Port<T, 1>::operator->()
{
    return (p_data != nullptr)? p_data : GetDataPointer();
}

template<typename T>
Module* Port<T, 1>::GetModulePointer()
{
    return (p_port == nullptr)? p_module : p_port->GetModulePointer();
}

template<typename T>
void Port<T, 1>::PointTo(Port<T>& another)
{
    p_data = nullptr;
    p_port = &another;
    p_module = nullptr;
}

template<typename T>
void Port<T, 1>::operator<<(Port<T>& another)
{
    p_data = nullptr;
    p_port = &another;
    p_module = nullptr;
}

template<typename T>
Port<T>& Port<T, 1>::PointTo(T& data)
{
    p_data = &data;
    p_port = nullptr;
    p_module = nullptr;
    return *this;
}

template<typename T>
Port<T>& Port<T, 1>::operator<<(T& data)
{
    p_data = &data;
    p_port = nullptr;
    p_module = nullptr;
    return *this;
}

template<typename T>
void Port<T, 1>::SetModulePtr(Module* pointer)
{
    this->p_module = pointer;
}

template<typename T>
void Port<T, 1>::operator<<(Module* pointer)
{
    this->p_module = pointer;
}

template<typename T>
bool Port<T, 1>::IsNull()
{
    return (p_data == nullptr && p_port == nullptr && p_module == nullptr);
}

template<typename T>
bool Port<T, 1>::Optimize()
{
    /* Back trace ports */
    p_module = GetModulePointer();
    p_data = GetDataPointer();
    
    /* Set as first port */
    p_port = nullptr;

    return IsNull();
}

/*******************************************************************************
* HANDLER SPECIALIZATIONS
********************************************************************************/

/* Specialization of Handler<Port<T>> */
#include "handler_port.hpp"

/* Specialization of Handler<Port<complex<T>>> */
#include "handler_port_complex.hpp"

/* Specialization of Handler<Port<ac_fixed<W, I, S, Q, O>>> */
#include "handler_port_ac_fixed.hpp"

/*******************************************************************************
* HANDLER GETTER
********************************************************************************/

template<typename T>
std::shared_ptr<AbstractHandler> Port<T, 1>::GetHandlerPointer()
{
    std::shared_ptr<AbstractHandler> ptr = std::make_shared<Handler<Port<T>>>();
    ptr->SetPointer(this);
    return ptr;
}