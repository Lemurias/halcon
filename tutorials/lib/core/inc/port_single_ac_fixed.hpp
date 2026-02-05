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
* Date: 05/27/2024
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
#include "ac_fixed.h"

/*******************************************************************************
* AC FIXED PORT SPECIALIZATED CLASS
********************************************************************************/

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
class Port<ac_fixed<W, I, S, Q, O>, 1> : public AbstractPort
{
private:

    /* Port definition */
    ac_fixed<W, I, S, Q, O>* p_data { nullptr };
    Port<ac_fixed<W, I, S, Q, O>>* p_port { nullptr };
    Module* p_module { nullptr };

    /* Control variables */
    bool p_data_is_dynamic { false };

public:

    Port() = default;
    ~Port();

    /* Data access */
    ac_fixed<W, I, S, Q, O> GetData();
    void SetData(ac_fixed<W, I, S, Q, O> data);
    
    /* Load data to variable */
    template <typename U>
    void LoadDataTo(U& data);
    
    template <typename U>
    void operator>>(U& data);

    /* Run methods through port */
    ac_fixed<W, I, S, Q, O>* GetDataPointer();
    ac_fixed<W, I, S, Q, O>* operator->();

    /* Point to another port */
    void PointTo(Port<ac_fixed<W, I, S, Q, O>>& another);
    void operator<<(Port<ac_fixed<W, I, S, Q, O>>& another);
    
    /* Point to data */
    Port<ac_fixed<W, I, S, Q, O>>& PointTo(ac_fixed<W, I, S, Q, O>& data);
    Port<ac_fixed<W, I, S, Q, O>>& operator<<(ac_fixed<W, I, S, Q, O>& data);

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

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
Port<ac_fixed<W, I, S, Q, O>, 1>::~Port()
{
    if(p_data_is_dynamic)
    {
        delete p_data;
    }
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
ac_fixed<W, I, S, Q, O> Port<ac_fixed<W, I, S, Q, O>, 1>::GetData()
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
        return 0;
    }

    /* Port pointing to data */
    return *(p_data);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Port<ac_fixed<W, I, S, Q, O>, 1>::SetData(ac_fixed<W, I, S, Q, O> data)
{
    if (p_data != nullptr)
    {
        *this->p_data = data;
        return;
    }
    
    p_data = new ac_fixed<W, I, S, Q, O>(data);
    p_data_is_dynamic = true;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
template <typename U>
void Port<ac_fixed<W, I, S, Q, O>, 1>::LoadDataTo(U& data)
{
    data = static_cast<U>(this->GetData().to_double());
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
template <typename U>
void Port<ac_fixed<W, I, S, Q, O>, 1>::operator>>(U& data)
{
    data = static_cast<U>(this->GetData().to_double());
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
ac_fixed<W, I, S, Q, O>* Port<ac_fixed<W, I, S, Q, O>, 1>::GetDataPointer()
{
    return (p_port == nullptr)? p_data : p_port->GetDataPointer();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
ac_fixed<W, I, S, Q, O>* Port<ac_fixed<W, I, S, Q, O>, 1>::operator->()
{
    return (p_data != nullptr)? p_data : GetDataPointer();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
Module* Port<ac_fixed<W, I, S, Q, O>, 1>::GetModulePointer()
{
    return (p_port == nullptr)? p_module : p_port->GetModulePointer();
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Port<ac_fixed<W, I, S, Q, O>, 1>::PointTo(Port<ac_fixed<W, I, S, Q, O>>& another)
{
    p_data = nullptr;
    p_port = &another;
    p_module = nullptr;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Port<ac_fixed<W, I, S, Q, O>, 1>::operator<<(Port<ac_fixed<W, I, S, Q, O>>& another)
{
    p_data = nullptr;
    p_port = &another;
    p_module = nullptr;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
Port<ac_fixed<W, I, S, Q, O>>& Port<ac_fixed<W, I, S, Q, O>, 1>::PointTo(ac_fixed<W, I, S, Q, O>& data)
{
    p_data = &data;
    p_port = nullptr;
    p_module = nullptr;
    return *this;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
Port<ac_fixed<W, I, S, Q, O>>& Port<ac_fixed<W, I, S, Q, O>, 1>::operator<<(ac_fixed<W, I, S, Q, O>& data)
{
    p_data = &data;
    p_port = nullptr;
    p_module = nullptr;
    return *this;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Port<ac_fixed<W, I, S, Q, O>, 1>::SetModulePtr(Module* pointer)
{
    this->p_module = pointer;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
void Port<ac_fixed<W, I, S, Q, O>, 1>::operator<<(Module* pointer)
{
    this->p_module = pointer;
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool Port<ac_fixed<W, I, S, Q, O>, 1>::IsNull()
{
    return (p_data == nullptr && p_port == nullptr && p_module == nullptr);
}

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool Port<ac_fixed<W, I, S, Q, O>, 1>::Optimize()
{
    /* Back trace ports */
    p_module = GetModulePointer();
    p_data = GetDataPointer();
    
    /* Set as first port */
    p_port = nullptr;

    return IsNull();
}

/*******************************************************************************
* HANDLER GETTER
********************************************************************************/

template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
std::shared_ptr<AbstractHandler> Port<ac_fixed<W, I, S, Q, O>, 1>::GetHandlerPointer()
{
    std::shared_ptr<AbstractHandler> ptr = std::make_shared<Handler<Port<ac_fixed<W, I, S, Q, O>>>>();
    ptr->SetPointer(this);
    return ptr;
}