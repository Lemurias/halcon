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

#include "basic_reflection.hpp"

class AbstractRegister;
class AbstractPort;

using RegistersMap = std::unordered_map<std::string, AbstractRegister*>;
using PortsMap = std::unordered_map<std::string, AbstractPort*>;

/**
 * @brief Base class of the framework. This class allows Module and Clock
 * instances to be able to "know" what it has instantiated inside (modules, 
 * clocks, variables, ports)
 * 
 */
class IntermediateReflection : public BasicReflection
{
protected:

    IntermediateReflection() = default;
    virtual ~IntermediateReflection() = default;
    
    /* Maps */
    RegistersMap reg_map;
    // HandlersMap h_reg_map;
    
    PortsMap port_map;
    HandlersMap h_port_map;

    /* Filters */
    template <typename T>
        requires std::is_base_of_v<AbstractRegister, T>
    void Reflect(std::string key, T& var);

    template <typename T>
        requires std::is_base_of_v<AbstractPort, T>
    void Reflect(std::string key, T& var);

    /* Enable the use of Reflect() of the BasicReflection class */
    using BasicReflection::Reflect;

public:

    virtual HandlersMap GetMapPointers(KeyFormat f=Full) const override;
    virtual std::map<std::string, std::string> GetMapTypes(KeyFormat f=Full) const override;
};

/**
 * @brief Reflects the name and the pointer of a register in the register map
 * 
 * @param key Register instance name
 * @param var Register instance reference
 */
template <typename T>
    requires std::is_base_of_v<AbstractRegister, T>
void IntermediateReflection::Reflect(std::string key, T& var)
{
    this->reg_map[key] = &var;
    // this->h_reg_map[key] = var.GetHandlerPointer();
}

/**
 * @brief Reflects the name and the pointer of a node in the node map
 * 
 * @param key Port name
 * @param var Port reference
 */
template <typename T>
    requires std::is_base_of_v<AbstractPort, T>
void IntermediateReflection::Reflect(std::string key, T& var)
{
    this->port_map[key] = &var;
    this->h_port_map[key] = var.GetHandlerPointer();
}