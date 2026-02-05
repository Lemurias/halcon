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

#include "intermediate_reflection.hpp"

class Module;
class Clock;

using ModulesMap = std::unordered_map<std::string, Module*>;
using ClocksMap = std::unordered_map<std::string, Clock*>;

/**
 * @brief Base class of the framework. This class allows Module and Clock
 * instances to be able to "know" what it has instantiated inside (modules, 
 * clocks, variables, ports)
 * 
 */
class AdvancedReflection : public IntermediateReflection
{
protected:

    AdvancedReflection() = default;
    virtual ~AdvancedReflection() = default;
    
    /* Maps */
    ModulesMap module_map;

    ClocksMap clock_map;
    HandlersMap h_clock_map;

    /* Filters */
    template <typename T>
        requires std::is_base_of_v<Module, T>
    void Reflect(std::string key, T& var);

    template <typename T>
        requires std::is_base_of_v<Clock, T>
    void Reflect(std::string key, T& var);

    /* Enable the use of Reflect() of the IntermediateReflection class */
    using IntermediateReflection::Reflect;

public:

    HandlersMap GetMapPointers(KeyFormat f=Full) const override;
    std::map<std::string, std::string> GetMapTypes(KeyFormat f=Full) const override;
};

/**
 * @brief Reflects the name and the pointer of a module in the modules map
 * 
 * @tparam T Module type
 * @param key Module name
 * @param var Module reference
 */
template <class T>
    requires std::is_base_of_v<Module, T>
void AdvancedReflection::Reflect(std::string key, T& var)
{
    this->module_map[key] = &var;
}

/**
 * @brief Reflects the name and the pointer of a clock in the clocks map
 * 
 * @param key Clock instance name
 * @param var Clock instance referece
 */
template <typename T>
    requires std::is_base_of_v<Clock, T>
void AdvancedReflection::Reflect(std::string key, T& var)
{
    this->clock_map[key] = &var;
    this->h_clock_map[key] = std::make_shared<Handler<Clock>>(var); 
}